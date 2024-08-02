import asyncio
import websockets
import logging
import keyboard
import ctypes

# Konfigurasi logger
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Variabel untuk menyimpan status jendela PowerPoint
is_open = True
# keyboard = Controller()

async def handle_connection(websocket, path):
    global is_open
    try:
        async for message in websocket:
            # Switch case the message
            match message:
                case "Hello Lentung":
                    response = {"message": "Hello Lentungers"}
                case "left":
                    if is_open:
                        keyboard.press_and_release('left')
                        response = {"message": "Pressed left arrow key"}
                    else:
                        response = {"message": "!Pressed left arrow key"}
                case "right":
                    if is_open:
                        keyboard.press_and_release('right')
                        response = {"message": "Pressed right arrow key"}
                    else:
                        response = {"message": "!Pressed right arrow key"}
                case "swipe":
                    if is_open:
                        keyboard.press_and_release('ctrl+windows+left')
                        response = {"message": "!Presentating"}
                        is_open = False
                    else:
                        keyboard.press_and_release('ctrl+windows+right')
                        response = {"message": "Presentating"}
                        is_open = True
                        
                    
                case _:
                    response = {"error": "Unknown command"}

            logging.info(f"Response: {response}")
            await websocket.send(str(response))

    except Exception as e:
        error_message = {"error": str(e)}
        logging.error(error_message)
        await websocket.send(str(error_message))

async def main():
    async with websockets.serve(handle_connection, "0.0.0.0", 8765):
        logging.info("WebSocket server started")
        await asyncio.Future()  # Run forever

if __name__ == '__main__':
    asyncio.run(main())
