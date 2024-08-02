import asyncio
import websockets
import json

async def handle_connection(websocket, path):
    # Print information about the new connection and path
    print(f"New connection from {websocket.remote_address} with path: {path}")

    if path == "/test":
        print("Client connected to /test path")
        test_json = {"message": "This is a test JSON message"}
        await websocket.send(json.dumps(test_json))  # Send test JSON to client

    elif path == "/test/stream":
        print("Client connected to /test/stream path")
        for i in range(5):  # Send 5 messages as an example
            message = json.dumps({"message": f"This is a test JSON message"})
            await websocket.send(message)
            await asyncio.sleep(1)  # 1 second interval between messages
        print("Finished sending stream messages")

    elif path == "/test/request":
        print("Client connected to /test/request path")
        try:
            async for message in websocket:
                print(f"Received message on /test/request path: {message}")
                await websocket.send(f"Echo: {message}")
        except websockets.ConnectionClosed:
            print(f"Connection closed from {websocket.remote_address}")
    
    elif path == "/test/transaction":
        print("Client connected to /test/transaction path")
        try:
            async for message in websocket:
                print(f"Received message on /test/traansaction path: {message}")
                message = json.dumps({"message": f"This is a test JSON message"})
                await websocket.send(message)
        except websockets.ConnectionClosed:
            print(f"Connection closed from {websocket.remote_address}")

    else:
        print(f"Unknown path: {path}")

async def main():
    # Set up the WebSocket server
    async with websockets.serve(handle_connection, "localhost", 8765):
        print("WebSocket server is running on ws://localhost:8765")
        await asyncio.Future()  # Run forever

if __name__ == "__main__":
    asyncio.run(main())
