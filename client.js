const protobuf = require("protobufjs");
const net = require("net");
const { nam } = require("./authentication.js");

// Load the proto file and create a root object
const root = protobuf.loadSync("authentication.proto");
const Authentication = root.lookup("nam");

// Define the server address and port
const SERVER_IP = "127.0.0.1";
const PORT = 12345;

const sendLoginRequest = (client) => {
  // Create a LoginRequest message
  const loginRequest = {
    username: "nam",
    password: "123",
  };

  // Create a BaseMessage with the LoginRequest
  const baseMessage = {
    messageType: 1, // Update with the correct message type value
    loginRequest: loginRequest,
  };
  console.log('baseMessage', baseMessage);
  // Encode the BaseMessage
  const encodedMessage = Authentication.BaseMessage.encode(baseMessage).finish();

  // Send the serialized message to the server
  client.write(encodedMessage);
  console.log(`Sent ${encodedMessage.length} bytes to server`);
};

const sendLogoutRequest = (client) => {
  // Create a LogoutRequest message
  const logoutRequest = {
    username: "nam",
  };
  const baseMessage = {
    messageType: 2, // Update with the correct message type value
    logoutRequest: logoutRequest,
  };
  // Encode the LogoutRequest message
  const encodedMessage = Authentication.BaseMessage.encode(baseMessage).finish();

  // Send the serialized message to the server
  client.write(encodedMessage);
  console.log(`Sent ${encodedMessage.length} bytes to server`);

};

const client = new net.Socket();

client.connect(PORT, SERVER_IP, () => {
  console.log("Connected to server");

  // Send a login request to the server
 // send 10 random login or logout request in 20s
  for(let i = 0; i < 10; i++) {
    setTimeout(() => {
      if(i % 2 === 0) {
        sendLoginRequest(client);
      } else {
        sendLogoutRequest(client);
      }
    }, i * 2000);
  }
  // sendLogoutRequest(client);
  // Send a logout request to the server
});

// Receive and process server response here
client.on("data", (data) => {
  // Decode the received data
  console.log('Raw data:', data);
  const decodedData = Authentication.BaseMessage.decode(data);
  console.log('Decoded data:', decodedData);
  if(decodedData.messageType === 1) {
    console.log('Login response');
  }
  if(decodedData.messageType === 2) {
    console.log('Logout response');
  }

  // Process the authentication response
  if (decodedData.authenticationResponse.success === true) {
    console.log("Login success");
  } else {
    console.log("Login failed");
  }

  // Close the socket after processing the response
  client.end();
});

client.on("close", () => {
  console.log("Connection closed");
});
