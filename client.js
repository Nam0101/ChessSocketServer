const protobuf = require("protobufjs");
const net = require("net");
const { nam } = require("./authentication.js"); // Update with the correct path to your generated authentication.js file

// Load the proto file and create a root object
const root = protobuf.loadSync("authentication.proto"); // Update with the correct path to your .proto file
const Authentication = root.lookup("nam");

// Define the server address and port
const SERVER_IP = "127.0.0.1";
const PORT = 12345;

const sendLoginRequest = (client) => {
  // Create a LoginRequest message
  const loginRequest = {
    messagetype: Buffer.from([1, 2, 3]), // Replace with your actual data
    username: "your_username",
    password: "your_password",
  };

  // Encode the message
  const encodedMessage = Authentication.LoginRequest.encode(loginRequest).finish();

  // Send the serialized message to the server
  client.write(encodedMessage);
  console.log(`Sent ${encodedMessage.length} bytes to server`);
};

const client = new net.Socket();

client.connect(PORT, SERVER_IP, () => {
  console.log("Connected to server");

  // Send a login request to the server
  sendLoginRequest(client);
});

// Receive and process server response here
client.on("data", (data) => {
  // Decode the received data
    console.log('Raw data:', data);
  const decodedData = Authentication.AuthenticationResponse.decode(data);

  // Process the authentication response
  console.log("Received server response:", decodedData);

  // Close the socket after processing the response
  client.end();
});

client.on("close", () => {
  console.log("Connection closed");
});
