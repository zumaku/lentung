import asyncio
import websockets
import logging
from pynput.keyboard import Controller, Key
from pywinauto import Desktop, Application

# Konfigurasi logger
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Variabel untuk menyimpan status jendela PowerPoint
is_open = True
keyboard = Controller()

def find_powerpoint_window():
    try:
        # Mencari jendela yang mengandung kata "PowerPoint" dalam judulnya
        windows = Desktop(backend="uia").windows(title_re=".*PowerPoint.*")
        if windows:
            return windows[0]
        else:
            return None
    except Exception as e:
        logging.error(f"Failed to find PowerPoint window: {str(e)}")
        return None

async def handle_connection(websocket, path):
    global is_open
    try:
        async for message in websocket:

            # Switch case the message
            match message:
                case "Hello Lentung":
                    response = {"message": "Hello Lentungers"}
                case "left":
                    keyboard.press(Key.left)
                    keyboard.release(Key.left)
                    response = {"message": "Pressed left arrow key"}
                case "right":
                    keyboard.press(Key.right)
                    keyboard.release(Key.right)
                    response = {"message": "Pressed right arrow key"}
                case "toggle_powerpoint":
                    if is_open:
                        # Keluar dari slideshow dengan ESC
                        keyboard.press(Key.esc)
                        keyboard.release(Key.esc)

                        app_window = find_powerpoint_window()
                        if not app_window:
                            response = {"error": "PowerPoint window not found"}
                        else:
                            app_window.minimize()
                            response = {"message": "Minimized PowerPoint window"}
                    else:
                        app_window = find_powerpoint_window()
                        if not app_window:
                            response = {"error": "PowerPoint window not found"}
                        else:
                            app_window.restore()
                            app_window.maximize()  # Memastikan jendela di-maximize
                            app_window.set_focus()

                            # Masuk kembali ke mode slideshow dengan F5
                            keyboard.press(Key.f5)
                            keyboard.release(Key.f5)

                            response = {"message": "Restored PowerPoint and entered slideshow mode"}

                    is_open = not is_open
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
