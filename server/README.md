# Server 
Asynchronous chat server using HTTP protocol and PostgreSQL 

# Dependencies

| Library  | Version  | 
| :------------: |:---------------| 
| [libpqxx](https://github.com/jtv/libpqxx)      | 6.0.0+ | 
| [cpprestsdk](https://github.com/microsoft/cpprestsdk)      | 2.10.0+ | 

# Deployment
1) Create database in pgAdmin
2) [Download the ready-made executable file](https://github.com/ROCKFAL1/http_chat/releases) or compile the sources yourself using the above dependencies
3) Run the terminal in the folder with the executable file 
4) Run the command `.\server.exe --port=<uint> --connstring=<str>`  where `--port=<uint>` - number of the port that will be listened to, and `--connstring=<str>` - connection string for your database. For example `.\server.exe --port=<8080> --connstring=<user=postgres host=localhost port=5432 password=your_password dbname=your_dbname>`
5) If the connection was successful, then the console will say <br>  
`Listening for requests at: http://localhost:8080/ `                                           
`Press ENTER to exit.` </br>
6) For convenience, you can create a script for the `start_server.bat` console and shove `.\server.exe --port=<8080> --connstring=<user=postgres host=localhost port=5432 password=your_password dbname=your_dbname>` into it
7) The necessary tables are created in the database and now you can accept connections and requests

# Requests
1) The request body always uses the JSON format  
2) To be considered an authorized user, it is necessary to keep a valid session_id in the request headers. You can get it by executing the correct POST request /login/
_______
# GET

| Request | Response | Do need to be logged in? |
| :------------: |:---------------| :---------------| 
| /conversations/  | Returns an array with the usernames of the users you have communicated with  | + |
| /conversations/<user_login> | Returns an array with the login of sender and message data | + |

# POST
| Request | What does it do? | Request body | Response  | Do need to be logged in? |
| :------------: |:---------------|:---------------| :---------------| :---------------| 
| /login/ | Login request |`{ "login": "login", "password": "password" }` |   Result and if success returns ` { "session_id": "some_session_id" } `  | - |
| /registration/ | Registration request |`{ "login": "login", "password": "password" }` | Result  | - |
| /conversations/add/<user_login> | Add new conversations with <user_login> request |- | Result | + |
| /conversations/<user_login> | Send message to <user_login> |` { "message": "some_message" } ` | Result | + |

