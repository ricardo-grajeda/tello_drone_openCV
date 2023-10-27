#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

#define port 8889
string ip = "192.168.10.1";
void direction(int dir[], int);
char buffer[1024];

int main()
{
	ZeroMemory(buffer, 1024);
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8889);
	inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);

	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	int sendok = sendto(out, "command", 8, 0, (sockaddr*)&hint, sizeof(hint));
	if (sendok == SOCKET_ERROR)
	{
		cout << "error" << endl;
	}


	sockaddr_in server_reply;
	ZeroMemory(&server_reply, sizeof(server_reply));
	int server_len = sizeof(server_reply);
	recvfrom(out, buffer, 1024, 0, (sockaddr*)&server_reply, &server_len);
	cout << "command: " << buffer << endl;
	Sleep(1000);
	sendto(out, "streamon", 9, 0, (sockaddr*)&hint, sizeof(hint));
	ZeroMemory(buffer, 1024);
	recvfrom(out, buffer, 1024, 0, (sockaddr*)&server_reply, &server_len);
	cout << "streamon: " << buffer << endl;



	char buff0[20];
	char buff1[20];
	char buff2[20];
	char buff3[20];

	const int size = 4;
	int dir[size] = { 0,0,0,0 };

	cv::VideoCapture capture{ "udp://0.0.0.0:11111", cv::CAP_FFMPEG };
	while (true)
	{

		cv::Mat frame;
		capture.read(frame);
		if (!frame.empty()) {
			cv::imshow("Tello Stream", frame);
		}

		cv::waitKey(1);

		sendto(out, "rc 0 0 0 0", 11, 0, (sockaddr*)&hint, sizeof(hint));

		ZeroMemory(buff0, 20);
		ZeroMemory(buff1, 20);
		ZeroMemory(buff2, 20);
		ZeroMemory(buff3, 20);

		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			sendto(out, "speed 10", 10, 0, (sockaddr*)&hint, sizeof(hint));

		}

		if (GetAsyncKeyState(VK_NUMPAD9) & 1)
		{
			sendto(out, "speed 100", 11, 0, (sockaddr*)&hint, sizeof(hint));

		}

		if (GetAsyncKeyState(VK_NUMPAD8) & 1)
		{
			sendto(out, "takeoff", 8, 0, (sockaddr*)&hint, sizeof(hint));

		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			sendto(out, "land", 5, 0, (sockaddr*)&hint, sizeof(hint));
		}
		if (GetAsyncKeyState(VK_HOME) & 1)
		{
			sendto(out, "command", 8, 0, (sockaddr*)&hint, sizeof(hint));
		}

		if (GetAsyncKeyState(VK_NUMPAD9) & 1)
		{
			sendto(out, "streamon", 9, 0, (sockaddr*)&hint, sizeof(hint));
			cout << "streaming starting" << endl;
		}
		direction(dir, size);

		_itoa_s(dir[0], buff0, 10);
		_itoa_s(dir[1], buff1, 10);
		_itoa_s(dir[2], buff2, 10);
		_itoa_s(dir[3], buff3, 10);

		string temp = "rc ";
		string order = temp + buff0 + " " + buff1 + " " + buff2 + " " + buff3;


		sendto(out, order.c_str(), sizeof(order), 0, (sockaddr*)&hint, sizeof(hint));
		//cout << order << endl;

		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}
		Sleep(1);

	}
	closesocket(out);
	WSACleanup();
	return 0;
}

void direction(int dir[], int size)
{
	int lr = 0, fb = 0, ud = 0, yv = 0;
	int speed = 100;
	if (GetAsyncKeyState(VK_LEFT)) { lr = -speed; }
	if (GetAsyncKeyState(VK_RIGHT)) { lr = speed; }

	if (GetAsyncKeyState(0x57)) { fb = speed; }
	if (GetAsyncKeyState(0x53)) { fb = -speed; }

	if (GetAsyncKeyState(VK_SPACE)) { ud = speed; }
	if (GetAsyncKeyState(VK_BACK)) { ud = -speed; }

	if (GetAsyncKeyState(0x44)) { yv = speed; }
	if (GetAsyncKeyState(0x41)) { yv = -speed; }

	dir[0] = lr;
	dir[1] = fb;
	dir[2] = ud;
	dir[3] = yv;

	return;
}