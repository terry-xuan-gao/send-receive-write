#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock.h>
#include <mysql.h>
#include <vector>

#include <iostream>
#include <queue>
#include <process.h>
#include <chrono>

using namespace std;

string version = "version 2.0 多线程版";

HANDLE hSerial;
DCB dcbSerialParams;
COMMTIMEOUTS timeouts;
MYSQL mysql;
MYSQL* conn;


std::queue<std::vector<string>> sharedQueue;
HANDLE hDataAvailable;
CRITICAL_SECTION cs;
bool isRunning = true;

vector<vector<double>> A = {
	{-0.069699439, 0.035721667, 0.029486,    -0.007688913},
	{-0.005092619, 0.108994307, -0.006549272, 0.001561142},
	{-0.002726086, 0.00097024,  0.064649797, -0.001321194},
	{0.250519428,  0.059926057, 0.110398148,  1.150738892}
};

vector<double> B = {
	48.36806825,
	-288.1076172,
	-209.0685374,
	-3527.950973
};

int init_mysql() {

	mysql_init(&mysql);

	//连接到MySQL数据库

	conn = mysql_init(NULL);
	if (conn == NULL) {
		std::cout << "MySQL初始化失败" << std::endl;
		CloseHandle(hSerial);
		return 1;
	}
	if (mysql_real_connect(conn, "localhost", "root", "139469", "fsw", 0, NULL, 0) == NULL) {
		std::cout << "MySQL连接失败： " << mysql_error(conn) << std::endl;
		mysql_close(conn);
		CloseHandle(hSerial);
		return 1;
	}
	cout << "MySQL连接成功" << endl;
	return 0;
}

int init_serial() {
	// 打开串口
	hSerial = CreateFile(TEXT("COM7"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSerial == INVALID_HANDLE_VALUE) {
		std::cout << "无法打开串口" << std::endl;
		return 1;
	}

	// 设置串口参数
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		std::cout << "无法获取串口参数" << std::endl;
		CloseHandle(hSerial);
		return 1;
	}
	dcbSerialParams.BaudRate = CBR_115200; // 设置波特率为115200
	dcbSerialParams.ByteSize = 8; // 设置数据位为8位
	dcbSerialParams.StopBits = ONESTOPBIT; // 设置停止位为1位
	dcbSerialParams.Parity = NOPARITY; // 设置无奇偶校验
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		std::cout << "无法设置串口参数" << std::endl;
		CloseHandle(hSerial);
		return 1;
	}

	// 设置串口读取超时时间
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(hSerial, &timeouts)) {
		std::cout << "无法设置串口超时时间" << std::endl;
		CloseHandle(hSerial);
		return 1;
	}

	cout << "串口打开成功" << endl;

	return 0;
}

void strs_to_numbers(const std::vector<string> strs)
{
	vector<int> fin(4, 0.0);
	for (int i = 0; i < 4; i++) {
		fin[i] = atoi(strs[i].c_str());
	}

	vector<double> fout(4, 0.0);
	for (int i = 0; i < 4; i++) {
		fout[i] = fin[0]*A[i][0] + fin[1]*A[i][1] +
			fin[2]*A[i][2] + fin[3]*A[i][3] + B[i];
	}

	cout << "Fz = " << fout[0] << " kN" << endl;
	cout << "Fxy = " << sqrt(fout[1]*fout[1] + fout[2]*fout[2]) << " kN" << endl;
	cout << "T = " << fout[3] << " N*m" << endl;
}

std::string process_message(const std::string& message) {

	size_t pos = message.rfind("f");
	if (pos != std::string::npos) {
		return " " + message.substr(pos);
	}
	return "";
}

bool producer(std::string& message) {
	std::cout << "message = " << message << endl;
	if (message.size() < 18) return false;

	std::vector<string> strs;

	if (message.size() != 18) {
		message = process_message(message);
		std::cout << "	修正 message = " << message << endl;

		if (message.size() != 18)
			return false;
	}
		
	for (int i = 2; strs.size() < 4; i+= 4) {
		string str = message.substr(i, 4);
		strs.push_back(str);
	}

	EnterCriticalSection(&cs);
	sharedQueue.push(strs);
	LeaveCriticalSection(&cs);
	SetEvent(hDataAvailable);

	return true;
}

unsigned __stdcall consumer(void*) {
	while (true) {

		WaitForSingleObject(hDataAvailable, INFINITE);
		EnterCriticalSection(&cs);

		if (!sharedQueue.empty()) {
			// 从队列中取出元素
			auto strs = sharedQueue.front();
			sharedQueue.pop();
			LeaveCriticalSection(&cs);

			std::string query = "INSERT INTO received_data_0 (Fz, Fx, Fy, T) VALUES ("
				+ strs[0] + "," + strs[1] + "," + strs[2] + "," + strs[3] + ")";
			std::cout << "	" << query << std::endl;
			if (mysql_query(conn, query.c_str())) {
				std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
			}
			Sleep(5);
		}
		else {
			LeaveCriticalSection(&cs);
		}
	}
	return 0;
}


int main()
{
	cout << version << endl;
	if (init_serial() == 1 || init_mysql() == 1) {
		return 0;
	}

	// 初始化临界区
	InitializeCriticalSection(&cs);
	// 创建事件
	hDataAvailable = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hConsumerThread = (HANDLE)_beginthreadex(NULL, 0, consumer, NULL, 0, NULL);
	
	char data;
	DWORD bytesRead;

	string message = "";
	cout << "开始接收数据" << endl;
	int dataGet = 0;
	while (true) {
		if (ReadFile(hSerial, &data, 1, &bytesRead, NULL)) {
			if (bytesRead > 0) {

				string str = string(1, data);

				if (str != "e")
					message += str;
				else
				{
					bool success = producer(message);
					if(!success) std::cout << "	数据[" << message << "]插入失败" << endl;
					message = "";
				}
			}
		}
	}

	CloseHandle(hSerial);

	return 0;
}