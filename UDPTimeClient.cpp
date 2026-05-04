#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include "Protocols.h"

using namespace std;

#define TIME_PORT	27015

int main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return 1;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255] = "What's the time?";
	char recvBuff[255];

	while (true)
    {
		// Show the menu to the user
        cout << "\n=== UDP Time Client ===\n";
        cout << " 1.  GetTime\n";
        cout << " 2.  GetTimeWithoutDate\n";
        cout << " 3.  GetTimeSinceEpoch\n";
        cout << " 4.  GetClientToServerDelayEstimation\n";
        cout << " 5.  MeasureRTT\n";
        cout << " 6.  GetTimeWithoutDateOrSeconds\n";
        cout << " 7.  GetYear\n";
        cout << " 8.  GetMonthAndDay\n";
        cout << " 9.  GetSecondsSinceBeginingOfMonth\n";
        cout << " 10. GetWeekOfYear\n";
        cout << " 11. GetDaylightSavings\n";
        cout << " 12. GetTimeWithoutDateInCity\n";
        cout << " 13. MeasureTimeLap\n";
        cout << " 14. Exit\n\n";
        cout << "Select option (number only): ";

        int choice;
        cin >> choice;

		if (choice == 14)
            break;

		// Translate choice to protocol string
        const char* request = nullptr;
        switch (choice)
        {
            case 1:  	request = Protocols::GET_TIME;
						break;
            case 2:  	request = Protocols::GET_TIME_WITHOUT_DATE;
						break;
            case 3:  	request = Protocols::GET_TIME_SINCE_EPOCH;
						break;
            case 4: {
						const int  NUM_REQUESTS = 100;
						DWORD      timestamps[NUM_REQUESTS];
						int        bytesRecv = 0;
						char       recvBuff[255];

						// sending 100 request to the server
						cout << "Sending " << NUM_REQUESTS << " requests...\n";
						for (int i = 0; i < NUM_REQUESTS; i++)
						{
							sendto(connSocket, Protocols::GET_CLIENT_TO_SERVER_DELAY,
								(int)strlen(Protocols::GET_CLIENT_TO_SERVER_DELAY),
								0, (const sockaddr*)&server, sizeof(server));
						}

						// receiving 100 response from the server and saves the timestamps
						cout << "Receiving " << NUM_REQUESTS << " responses...\n";
						for (int i = 0; i < NUM_REQUESTS; i++)
						{
							bytesRecv = recv(connSocket, recvBuff, 255, 0);
							recvBuff[bytesRecv] = '\0';
							timestamps[i] = strtoul(recvBuff, nullptr, 10);
						}

						// calculating average difference between consecutive timestamps
						double totalDiff = 0;
						for (int i = 0; i < NUM_REQUESTS - 1; i++)
							totalDiff += timestamps[i + 1] - timestamps[i];

						double avgDiff = totalDiff / (NUM_REQUESTS - 1);
						cout << "Average time between consecutive server timestamps: "
							<< avgDiff << " ms\n";

						continue;
					}
            case 5:
					{
						const int NUM_REQUESTS = 100;
						char      recvBuff[255];
						int       bytesRecv = 0;
						double    totalRTT  = 0;

						cout << "Measuring RTT over " << NUM_REQUESTS << " requests...\n";

						for (int i = 0; i < NUM_REQUESTS; i++)
						{
							// measure time before sending
							DWORD t1 = GetTickCount();

							// sending a request
							sendto(connSocket, Protocols::MEASURE_RTT,
								(int)strlen(Protocols::MEASURE_RTT),
								0, (const sockaddr*)&server, sizeof(server));

							// waiting for response
							bytesRecv = recv(connSocket, recvBuff, 255, 0);
							recvBuff[bytesRecv] = '\0';

							// measure time after receiving response
							DWORD t2 = GetTickCount();

							totalRTT += (t2 - t1);
						}

						double avgRTT = totalRTT / NUM_REQUESTS;
						cout << "Average RTT: " << avgRTT << " ms\n";

						continue;
					}
            case 6:  	request = Protocols::GET_TIME_WITHOUT_DATE_OR_SECONDS;
						break;
            case 7:  	request = Protocols::GET_YEAR;
						break;
            case 8:  	request = Protocols::GET_MONTH_AND_DAY;
						break;
            case 9:  	request = Protocols::GET_SECONDS_SINCE_MONTH_START;
						break;
            case 10: 	request = Protocols::GET_WEEK_OF_YEAR;
						break;
            case 11: 	request = Protocols::GET_DAYLIGHT_SAVINGS;
						break;
            case 12: {
						// Send the GetTimeInCity request to the server.
						request = Protocols::GET_TIME_IN_CITY;
						bytesSent = sendto(connSocket, request, (int)strlen(request), 0, (const sockaddr*)&server, sizeof(server));

						bytesRecv = recv(connSocket, recvBuff, 255, 0);
						recvBuff[bytesRecv] = '\0';
						cout << recvBuff; // Display the list of cities

						// Receive the city name from the user and send it to the server.
						char cityName[255];
						cin.ignore();
						cin.getline(cityName, 255);

						bytesSent = sendto(connSocket, cityName, (int)strlen(cityName), 0, (const sockaddr*)&server, sizeof(server));

						bytesRecv = recv(connSocket, recvBuff, 255, 0);
						recvBuff[bytesRecv] = '\0';
						cout << "Time in " << cityName << ": " << recvBuff << endl;
						continue;
					}

            case 13:
					{
						char recvBuff[255];
						int  bytesRecv = 0;

						// send MeasureTimeLap request to the server
						sendto(connSocket, Protocols::MEASURE_TIME_LAP,
							(int)strlen(Protocols::MEASURE_TIME_LAP),
							0, (const sockaddr*)&server, sizeof(server));

						// receive response from server
						bytesRecv = recv(connSocket, recvBuff, 255, 0);
						recvBuff[bytesRecv] = '\0';
						cout << "Server: " << recvBuff << endl;

						continue;
					}
            default:
                cout << "Invalid option, please try again.\n";
                continue;
        }

		// Asks the server what's the currnet time.
		// The send function sends data on a connected socket.
		// The buffer to be sent and its size are needed.
		// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
		// The two last arguments hold the details of the server to communicate with. 
		// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
		bytesSent = sendto(connSocket, request, (int)strlen(request), 0, (const sockaddr *)&server, sizeof(server));
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return 1;
		}
		cout << "Time Client: Sent: " << bytesSent << "/" << strlen(request) << " bytes of \"" << request << "\" message.\n";

		// Gets the server's answer using simple recieve (no need to hold the server's address).
		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return 1;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	system("pause");

	return 0;
}