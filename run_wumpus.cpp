// Unix headers
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<signal.h>

// Network headers
#include<netinet/ip.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/socket.h>

// C++ headers
#include<string>
#include<iostream>
#include<cstdlib>

class NetworkWumpusGame {
	bool active = false;
	char *buffer;
	int skt;
	const int bufsize = 4096; // Long enough for anything the game prints

public:
	NetworkWumpusGame(){
		buffer = (char*)malloc(bufsize);
	}

	ssize_t do_action(std::string command){
		return write(skt, command.c_str(), command.length());
	}
	
	std::string retrieve_result(){
		// If you have weird problems that seem to have to do with incomplete output from wump, try this line
	//	usleep(30000); // Might need to be more than the network latency
		ssize_t readlen = read(skt, buffer, bufsize);
		buffer[readlen] = 0; // read doesn't add a null terminator
		if(readlen < 1){
			perror("read");
			return "";
		}
		return std::string(buffer);
	}
	
	void stop_wumpus(){
		close(skt);
		active = false;
	}
	
	void start_wumpus(){
		struct sockaddr_in sad;
		sad.sin_port = htons(5140);
		sad.sin_family = AF_INET;
		skt = socket(AF_INET, SOCK_STREAM, 0);
		struct hostent* entry = gethostbyname("isoptera.lcsc.edu");
		struct in_addr **addr_list = (struct in_addr**)entry->h_addr_list;
		struct in_addr* c_addr = addr_list[0];
		char* ip_string = inet_ntoa(*c_addr);
		sad.sin_addr = *c_addr;
		printf("Connecting to:  %s\n", ip_string);
	
		connect(skt, (struct sockaddr*)&sad, sizeof(struct sockaddr_in));

		active = true; // Unless it failed, but we don't check that!
	}		

	~NetworkWumpusGame(){
		free(buffer);
		if(active)
			stop_wumpus();
	}
};

class WumpusGame {
	// Pipes are unidirectional, so we need one for each direction
	int output_pipe_fds[2];
	int input_pipe_fds[2];
	bool active = false;
	pid_t wumpus_pid;
	char *buffer;
	const int bufsize = 4096; // Long enough for anything the game prints

public:
	WumpusGame(){
		buffer = (char*)malloc(bufsize);
	}

	ssize_t do_action(std::string command){
		return write(input_pipe_fds[1], command.c_str(), command.length());
	}
	
	std::string retrieve_result(){
		/* usleep will give the game a chance to do the last action.  But if you want this to
		 * go faster, you could just check and make sure read actually gave you whatever you 
		 * were expecting.  read will block until there is something to read, so at worst you 
		 * could get a partial message with this usleep.  In fact, depending on how the game 
		 * is written, it might be fine to just remove it completely. */
//		usleep(2000);
		ssize_t readlen = read(output_pipe_fds[0], buffer, bufsize);
		buffer[readlen] = 0; // read doesn't add a null terminator
		if(readlen < 1){
			perror("read");
			return "";
		}
		return std::string(buffer);
	}
	
	void stop_wumpus(){
		if(kill(wumpus_pid, 9))
			perror("killing wumpus");
		wait(0); // Prevents zombies
		close(output_pipe_fds[0]);
		close(input_pipe_fds[1]);
		active = false;
	}
	
	void start_wumpus(){
		if(pipe(output_pipe_fds))
			perror("start_wumpus: pipe");
		if(pipe(input_pipe_fds))
			perror("start_wumpus: pipe");
		wumpus_pid = fork(); // Divide into two processes, parent and child.
		if(!wumpus_pid){ // if we're the child
			dup2(output_pipe_fds[1], 1);
			dup2(input_pipe_fds[0], 0);
			execlp("wump", "wump", 0); // Unless it fails, this won't return
			close(output_pipe_fds[0]); // At this point, we've failed to start wumpus
			close(input_pipe_fds[1]);
			perror("Spawing wumpus failed");
		}
		active = true; // Unless it failed, but we don't check that!
	}		

	~WumpusGame(){
		free(buffer);
		if(active)
			stop_wumpus();
	}
};

/* Demonstrates use of class WumpusGame*/
int main(){
	NetworkWumpusGame wg;
	wg.start_wumpus();
	
	std::cout << wg.retrieve_result();
	wg.do_action("n\n");
	std::string result = wg.retrieve_result();
	std::cout << result;
	int room = 1;
	//bool bats = false;
	int dist = 0;
	int proche =0 ;
	//int pos=0;
	int andpos=0;
	//int last = 0;
	int rs = 31;
	std::string room1, room2, room3,act_room,last_room;
		
	while(std::string::npos != result.find("You are in room")|| std::string::npos != result.find("To which room")|| std::string::npos != result.find("*Oof!*")){
				

		if(std::string::npos != result.find("To which room do you wish to move")){
			room =((rand()%3)+1);
				
			
				std::cout<<"in loop 1"<<std::endl;
				if(room == 1)
					wg.do_action("m 10\n");
				if(room == 2)
					wg.do_action(room2 + "\n");
				if(room ==3)
					wg.do_action(room3 + "\n");
			result = wg.retrieve_result();
			std::cout<<result<<std::endl;
		}
		if(std::string::npos != result.find("You are in room")){
							
				std::size_t position = result.find("rooms",rs);
				std::size_t coma = result.find(",",position);
				dist = coma-(position+5);
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room1 = result.substr(position+5,dist);
				std::cout<<std::endl<<"room one is ---->>>>>>>>>>"<<room1<<std::endl;
				std::size_t coma1 = result.find(",",coma+1);
				dist = coma1-coma-1;
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room2 = result.substr(coma+1,dist);
				std::cout<<std::endl<<"room two is->"<<room2<<"  "<<std::endl;
				std::size_t end = result.find(".",coma1+1);
				std::size_t andpos = result.find("and",coma1);
				dist= end-(andpos+3);
			//	std::cout<<std::endl<<std::endl<<dist<<"__>"<<std::endl;
				room3 = result.substr(andpos+3,dist);
				position = result.find("You are in room");
				act_room = result.substr(position+15,3);
				std::cout<<std::endl<<std::endl<<"the last visited room is "<<act_room<<std::endl;
			//	std::cout<<result<<std::endl;
				wg.do_action("m " + room2 + "\n");

				result = wg.retrieve_result();
				std::cout<<result<<std::endl;



		if(std::string::npos != result.find("sniff") && (proche==0)){
				std::cout<<"in loop 2a"<<std::endl;
				proche=1;

				std::size_t position = result.find("rooms",rs);
				std::size_t coma = result.find(",",position);
				dist = coma-(position+5);
				room1 = result.substr(position+5,dist);
				std::cout<<std::endl<<"room one is ---->>>>>>>>>>"<<room1<<std::endl;
				std::size_t coma1 = result.find(",",coma+1);
				dist = coma1-coma-1;
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room2 = result.substr(coma+1,dist);
				std::cout<<std::endl<<"room two is->"<<room2<<"  "<<std::endl;
				std::size_t end = result.find(".",coma1+1);
				std::size_t andpos = result.find("and",coma1);
				dist= end-(andpos+3);
			//	std::cout<<std::endl<<std::endl<<dist<<"__>"<<std::endl;
				room3 = result.substr(andpos+3,dist);



				result=wg.retrieve_result();
				std::cout<<result<<std::endl;
				if(std::string::npos ==result.find("sniff")&& std::string::npos == result.find("*flap*")){
					wg.do_action("m" + act_room + "\n");
					result = wg.retrieve_result();
					proche = 0;
					std::cout<<result<<std::endl;
								
			}
		}
		if(std::string::npos != result.find("sniff") && proche == 1){
				//get the actual room so that if the next room doesnt smell the wump you go back to the previous room.
				std::cout<<"in loop 2b"<<std::endl;

				std::size_t position = result.find("rooms",rs);
				std::size_t coma = result.find(",",rs);
				dist = coma-(position+5);
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room1 = result.substr(position+5,dist);
				std::cout<<std::endl<<"room one is ___>>>>>-----"<<room1<<std::endl;
				std::size_t coma1 = result.find(",",coma+1);
				dist = coma1-coma-1;
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room2 = result.substr(coma+1,dist);
				std::cout<<std::endl<<"room two is"<<room2<<std::endl;
				std::size_t end = result.find(".",coma1+1);
				std::size_t andpos = result.find("and",coma1);
				dist= end-(andpos+3);
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room3 = result.substr(andpos+3,dist);
				std::cout<<std::endl<<"room three is"<<room3<<std::endl;
				proche = 0;
					
				wg.do_action("s"+room1 + "\n");
				result = wg.retrieve_result();
				wg.do_action("s"+room2 + "\n");
				result = wg.retrieve_result();
				wg.do_action("s"+room3 + "\n");
				result = wg.retrieve_result();
				std::cout<<result<<std::endl;
			}
		

		}

		if(std::string::npos != result.find("To which room do you wish to move")){
			room = ((rand()%3)+1);

				
				std::cout<<"in           >>>>>>>>>>> loop to >>>>>>>>>>>> which"<<std::endl;
				if(room == 1)
					wg.do_action(room3 + "\n");
				if(room == 2)
					wg.do_action(room2 + "\n");
				if(room ==3)
					wg.do_action(room3 + "\n");
				result = wg.retrieve_result();
				//std::cout<<result<<std::endl;
		}
		if(std::string::npos != result.find("*flap*")){

				std::size_t position = result.find("rooms",80);
				std::size_t coma = result.find(",",80);
				dist = coma-(position+5);
				room1 = result.substr(position+5,dist);
				std::cout<<std::endl<<"room one is ---->>>>>>>>>>"<<room1<<std::endl;
				std::size_t coma1 = result.find(",",coma+1);
				dist = coma1-coma-1;
			//	std::cout<<std::endl<<std::endl<<dist<<std::endl;
				room2 = result.substr(coma+1,dist);
				std::cout<<std::endl<<"room two is->"<<room2<<"  "<<std::endl;
				std::size_t end = result.find(".",coma1+1);
				std::size_t andpos = result.find("and",coma1);
				dist= end-(andpos+3);
			//	std::cout<<std::endl<<std::endl<<dist<<"__>"<<std::endl;
				room3 = result.substr(andpos+3,dist);
				room = ((rand()%3)+1);
				if(room == 1)
					wg.do_action("m " + room1 + "\n");
				if(room == 2)
					wg.do_action("m " + room2 + "\n");
				if(room == 3)
					wg.do_action("m " + room3 + "\n");
				result = wg.retrieve_result();
		}



	}

	std::cout<<result<<std::endl;
	wg.stop_wumpus();

	return 0;
}

