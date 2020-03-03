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
    this->n = 0;
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
    while(this->n < 2){
        listen(SocketFileDescriptor, 5);
        ClientLength = sizeof(ClientAddress);

        NewSocketFileDescritor = accept(SocketFileDescriptor, (struct sockaddr *)&ClientAddress, &ClientLength);
        if(0 > NewSocketFileDescritor){
            perror("Error on Accept");
        }
        std::cout << "Connection statrted with" << ClientAddress.sin_addr.s_addr << std::endl;
        this->SocketFileDescriptors[n] = NewSocketFileDescritor;
        n++;
    }
    

    return;





}

void RemoteServer::TCPRead(){
    
    
   
    for(int i = 0; i < this->n; i++){
        int Results;
        char Buffer[PACKET_SIZE_MAX];
        Results = read(this->SocketFileDescriptors[i], Buffer, PACKET_SIZE_MAX-1);
        if (Results < 1){
            perror("Error Reading From Socket");
            return;
        }else if(Results == 0){
            perror ("Closing sockets");
            this->CloseSocket();
            return;
        }
        std::string temp(Buffer);
        std::cout << "Reading in " << Buffer<< std::endl;
        this->SerializedData[i] = temp;
        
    }


}

void RemoteServer::TCPWrite(){

    for(int i = 0; i < this->n; i++){
        for (int j = 0; j < this->n; j++){
            int Result;
            if(i != j){
                std::cout <<"Writing " <<  SerializedData[j].c_str() << " to " << i+1 << std::endl;
                Result = write(this->SocketFileDescriptors[i], this->SerializedData[j].c_str(), this->SerializedData[j].size());
                if (Result < 0) {
                    perror("Error on Writing");
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
    std::cout<<"n is :" << server.n << std::endl;
    server.ServerSetup(atoi(argv[1]));
    std::cout <<"Now n is :" << server.n << std::endl;
    while(true){
        server.TCPRead();
       server.TCPWrite();

    }


}