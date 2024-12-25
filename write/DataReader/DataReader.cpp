#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock.h>
#include <mysql.h>
#include <vector>

using namespace std;

std::vector<int> string_to_vector(const std::string& whole_data) {
	
	std::vector<int> data_vector;
	
	int count = 0;
	for (char c : whole_data)
		if (c == ',')
			count++;

	if (count != 8)
		return data_vector;
	
	
	std::stringstream ss(whole_data);
	std::string item;
	while (std::getline(ss, item, ',')) {
		if (item == "" || item == " ")
			item = "5432";
		data_vector.push_back(std::stoi(item));
	}
	return data_vector;
}

int main()
{
	MYSQL mysql;
	mysql_init(&mysql);
	HANDLE hSerial;
	DCB dcbSerialParams;
	COMMTIMEOUTS timeouts;

	// 打开串口
	hSerial = CreateFile(TEXT("COM5"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	//连接到MySQL数据库
	MYSQL* conn;
	conn = mysql_init(NULL);
	if (conn == NULL) {
		std::cout << "MySQL初始化失败" << std::endl;
		CloseHandle(hSerial);
		return 1;
	}
	if (mysql_real_connect(conn, "localhost", "root", "123456789", "test", 0, NULL, 0) == NULL) {
		std::cout << "MySQL连接失败： " << mysql_error(conn) << std::endl;
		mysql_close(conn);
		CloseHandle(hSerial);
		return 1;
	}
	// 读取并打印串口数据，并将数据插入到MySQL数据库中
	/*char data;
	DWORD bytesRead;
	while (true) {
		if (ReadFile(hSerial, &data, 1, &bytesRead, NULL)) {
			if (bytesRead > 0) {
				std::cout << data;

				// 将数据插入到MySQL数据库中
				std::string query = "INSERT INTO received_data_05 (data) VALUES ('" + std::string(1, data) + "')";
				if (mysql_query(conn, query.c_str())) {
					std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
				}
			}
		}
	}*/

	
	

	//while (true) {

	//	char data;
	//	DWORD bytesRead;
	//	std::string receivedData;

	//	while(true)
	//	{
	//		if (!ReadFile(hSerial, &data, 1, &bytesRead, NULL)) {
	//			std::cerr << "读取串口数据失败" << std::endl;
	//			break;
	//		}

	//		string data_str = string(1, data);

	//		cout << "data = " << data_str;
	//		if (data_str == "E")
	//			break;

	//		receivedData += data_str;
	//	}


	//	std::cout << "接收到的字符串： " << receivedData << std::endl;

	//	std::string query = "INSERT INTO received_data_01 (data) VALUES ('" + receivedData + "')";
	//	if (mysql_query(conn, query.c_str())) {
	//		std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
	//	}

	//}


	//char data;
	//DWORD bytesRead;

	//string whole_data = "";
	//int column_count = 0;
	//int row_count = 0;

	//while (true) {
	//	if (ReadFile(hSerial, &data, 1, &bytesRead, NULL)) {
	//		if (bytesRead > 0) {
	//			//std::cout << data;

	//			string str = string(1, data);
	//			cout << "str = " << str << endl;

	//			if (str != "E") {
	//				whole_data += str;
	//				column_count++;
	//			}
	//			else {
	//				// 将数据插入到MySQL数据库中

	//				cout << "whole_data = " << whole_data << endl;

	//				std::string query = "INSERT INTO received_data_00 (data_" + std::to_string(row_count * 8 + column_count) + ") VALUES ('" + whole_data + "')";
	//				if (mysql_query(conn, query.c_str())) {
	//					std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
	//				}

	//				whole_data = "";
	//				column_count = 0;
	//				if (column_count == 7) {
	//					row_count++;
	//					column_count = 0;
	//				}
	//			}
	//		}
	//	}
	//}




	char data;
	DWORD bytesRead;

	string whole_data = "";
	cout << "开始接收数据" << endl;
	int dataGet = 0;
	while (true) {
		if (ReadFile(hSerial, &data, 1, &bytesRead, NULL)) {
			if (bytesRead > 0) {
				//std::cout << data;
				

				string str = string(1, data);
				//cout << "str = " << str << endl;

				if (str != "E")
					whole_data += str;
				else
				{
					// 将数据插入到MySQL数据库
					
					

					std::vector<int> data_vector = string_to_vector(whole_data);
					dataGet += 1;
					cout << dataGet << endl;
					if (data_vector.size() == 8 && dataGet == 10)
					{
						string sendToMysql = "";

						cout << "whole_data = " << whole_data << endl;
						int value1 = data_vector[0] * 255 + data_vector[1];
						int value2 = data_vector[2] * 255 + data_vector[3];
						int value3 = data_vector[4] * 255 + data_vector[5];
						int value4 = data_vector[6] * 255 + data_vector[7];
						
						
						cout << "sendToMysql = " << value1 <<"," << value2 <<"," << value3 <<"," << value4 << endl;

						std::string query = "INSERT INTO received_data_0 (Fz, Fx, Fy, T) VALUES (" + std::to_string(value1) + "," + std::to_string(value2) + "," + std::to_string(value3) + "," + std::to_string(value4) + ")";
					  
						
						
						if (mysql_query(conn, query.c_str())) {
							std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
						}
						dataGet = 0;
					}
					whole_data = "";
				}

				
			}
		}
	}

	/*
	char data[1024]; // 定义一个字符数组，用于存储读取到的串口数据
	DWORD bytesRead;

	int whole_data = 0; // 将whole_data改为整数变量
	cout << "开始接收数据" << endl;
	while (true) {
		if (ReadFile(hSerial, &data, sizeof(data), &bytesRead, NULL)) {
			if (bytesRead > 0) {
				
				
				
				for (int i = 0; i < bytesRead / 2; i++) {
					
					
					whole_data = data[i * 2] * 255 + data[i * 2 + 1]; // 将读到的数据执行命令
					string str = to_string(whole_data); // 将整数转换为字符串
					cout << "str = " << str << endl;

					if (str != "E")
						whole_data += stoi(str); // 将字符串转换为整数并累加到whole_data中
					else {
						// 将数据插入到MySQL数据库中

						cout << "whole_data = " << whole_data << endl;

						std::string query = "INSERT INTO received_data_5 (data) VALUES ('" + to_string(whole_data) + "')";
						if (mysql_query(conn, query.c_str())) {
							std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
						}

						whole_data = 0; // 清空whole_data
					}
				}
			}
		}
	}
	*/


	/*std::string data;
	DWORD bytesRead;
	while (true) {
		if (ReadFile(hSerial, &data[0], 1, &bytesRead, NULL)) {
			if (bytesRead > 0) {
				std::cout << data;

				// 将数据插入到MySQL数据库中
				std::string query = "INSERT INTO received_data_01 (data) VALUES ('" + data + "')";
				if (mysql_query(conn, query.c_str())) {
					std::cout << "插入数据失败： " << mysql_error(conn) << std::endl;
				}
			}
		}
	}*/
	// 关闭串口
	CloseHandle(hSerial);

	return 0;
}