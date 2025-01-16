#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock.h>
#include <mysql.h>
#include <vector>

using namespace std;

HANDLE hSerial;
DCB dcbSerialParams;
COMMTIMEOUTS timeouts;
MYSQL mysql;
MYSQL* conn;

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

	return 0;
}

int init_serial() {
	// 打开串口
	hSerial = CreateFile(TEXT("COM9"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	return 0;
}

std::vector<string> splited_string(const std::string& message) {

	std::cout << message << endl;

	std::vector<string> strs;

	for (int i = 2; i <= 14; i+= 4) {

		string str = message.substr(i, 4);
		strs.push_back(str);
	}

	return strs;
}

int main()
{
	init_serial();
	init_mysql();
	
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
					// 将数据插入到MySQL数据库
					std::vector<string> strs = splited_string(message);
					dataGet += 1;
					
					if (strs.size() == 4 && dataGet == 1)
					{			
						cout << "sendToMysql = " << strs[0] <<"," << strs[1] <<"," 
							<< strs[2] <<"," << strs[3] << endl;

						std::string query = "INSERT INTO received_data_0 (Fz, Fx, Fy, T) VALUES ("
							+ strs[0] + "," + strs[1] + "," + strs[2] + "," + strs[3] + ")";
					  
						if (mysql_query(conn, query.c_str())) {
							std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
						}
						dataGet = 0;
					}
					
					message = "";
				}

				
			}
		}
	}

	CloseHandle(hSerial);

	return 0;
}