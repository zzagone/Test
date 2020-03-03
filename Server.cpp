#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <bits/stdc++.h>
#include <queue>


int PACKET_SIZE_MAX = 256;

class RemoteServer{
    public:
    RemoteServer();
    int n;
    int SocketFileDescriptors[6];
    void ServerSetup(int);
    void TCPRead();
    void TCPWrite();
    void CloseSocket();
    protected:
    std::string SerializedData[6];
};

RemoteServer::RemoteServer(){
    n = 0;
}

void RemoteServer::ServerSetup(int portnumber){

    int SocketFileDescriptor, NewSocketFileDescritor, Result;
    socklen_t ClientLength;
    struct sockaddr_in ServerAddress, ClientAddress;


    SocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(0 > SocketFileDescriptor){
        perror("Error Opening Socket");
    }

    bzero((char *) &ServerAddress, sizeof (ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(portnumber);

    if(0 > bind(SocketFileDescriptor, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))){
        perror("Error binding");
    }
    while(n < 2){
    listen(SocketFileDescriptor, 5);
    ClientLength = sizeof(ClientAddress);

    NewSocketFileDescritor = accept(SocketFileDescriptor, (struct sockaddr *)&ClientAddress, &ClientLength);
    if(0 > NewSocketFileDescritor){
        perror("Error on Accept");
    }
    std::cout << "Connection statrted with" << ClientAddress.sin_addr.s_addr << std::endl;
    this->SocketFileDescriptors[n] = NewSocketFileDescritor;
    n++
    }
    

    return;





}

void RemoteServer::TCPRead(){

    char Buffer[PACKET_SIZE_MAX];
    int Results;
    for(int i = 0; i < this->n; i++){
        Results = read(this->SocketFileDescriptors[i], Buffer, PACKET_SIZE_MAX -1);
        if(Results < 1){
            perror("Error Reading from Socket");
            return;
        } else if(Results == 0){
            perror("Closing sockets");
            this->CloseSocket();
            return;
        }
        std::string FullSerialized(Buffer);
        int NumberBytesInPacket = std::stoi(FullSerialized.substr(0,3));
        this->SerializedData[i] = FullSerialized.substr(4, NumberBytesInPacket);
        std::cout << this->SerializedData[i] << std::endl;
    }
    return;
}

void RemoteServer::TCPWrite(){
    int Result;
    char Prepend[4];
    std::string NewSerializedData[6];
    std::cout << "Test 1 n: " << this->n << std::endl;
    for(int i = 0; i < this->n; i++){
        sprintf(Prepend, "%03d|", this->SerializedData[i].size());
        std::string NewSerialized(Prepend);
        NewSerialized.append(this->SerializedData[i]);
        NewSerializedData[i] = NewSerialized;
    }

    for(int i = 0; i < this->n; i++){
        for(int j = 0; j < this->n; j++){
            if (i != j){
                Result = write(this->SocketFileDescriptors[i], NewSerializedData[j].c_str(), NewSerializedData[j].size());
                std::cout << "Writing " << NewSerializedData[j].c_str() << std::endl;
                if (Result < 0){
                    perror("Error Writing to Socket");
                }
            }
        }

    }
    
}
void RemoteServer::CloseSocket(){
    for(int i = 0; i < n; i++){
        close(this->SocketFileDescriptors[i]);
    }
}



int main(int argc, char *argv[]){

    std::cout << "starting the server..." << std::endl;
    RemoteServer server;
    std::cout<<"n is :" << server.n << endl;
    server.ServerSetup(atoi(argv[1]));
    std::cout <<"Now n is :" << server.n << endl;
    while(true){
        server.TCPRead();
        //server.TCPWrite();

    }


}