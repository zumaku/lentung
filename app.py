from flask import Flask, request, jsonify
from pynput.keyboard import Controller, Key
from pywinauto import Desktop, Application
import logging
import time

# Konfigurasi logger
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

app = Flask(__name__)
keyboard = Controller()

# Variabel untuk menyimpan status jendela PowerPoint
is_open = True

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

@app.before_request
def log_request_info():
    logging.info(f'Request Headers: {request.headers}')
    logging.info(f'Request Body: {request.get_data()}')

@app.after_request
def log_response_info(response):
    logging.info(f'Response Status: {response.status}')
    logging.info(f'Response Body: {response.get_data()}')
    return response

@app.route('/press_right', methods=['POST'])
def press_right():
    try:
        keyboard.press(Key.right)
        keyboard.release(Key.right)
        message = {"message": "Pressed right arrow key"}
        logging.info(message)
        return jsonify(message), 200
    except Exception as e:
        error_message = {"error": str(e)}
        logging.error(error_message)
        return jsonify(error_message), 500

@app.route('/press_left', methods=['POST'])
def press_left():
    try:
        keyboard.press(Key.left)
        keyboard.release(Key.left)
        message = {"message": "Pressed left arrow key"}
        logging.info(message)
        return jsonify(message), 200
    except Exception as e:
        error_message = {"error": str(e)}
        logging.error(error_message)
        return jsonify(error_message), 500

def find_powerpoint_window():
    try:
        # Mencari jendela utama PowerPoint
        app_window = Desktop(backend="uia").window(title_re=".*PowerPoint.*")
        return app_window if app_window.exists() else None
    except Exception as e:
        logging.error(f"Failed to find PowerPoint window: {str(e)}")
        return None

@app.route('/toggle_powerpoint', methods=['POST'])
def toggle_powerpoint():
    global is_open
    try:
        if is_open:
            # Keluar dari slideshow dengan ESC
            keyboard.press(Key.esc)
            keyboard.release(Key.esc)

            app_window = find_powerpoint_window()
            if not app_window:
                return jsonify({"error": "PowerPoint window not found"}), 404

            app_window.minimize()
            message = {"message": "Minimized PowerPoint window"}
        else:
            app_window = find_powerpoint_window()
            if not app_window:
                return jsonify({"error": "PowerPoint window not found"}), 404

            app_window.restore()
            app_window.maximize()  # Memastikan jendela di-maximize
            app_window.set_focus()

            # Masuk kembali ke mode slideshow dengan F5
            keyboard.press(Key.f5)
            keyboard.release(Key.f5)

            message = {"message": "Restored PowerPoint and entered slideshow mode"}

        is_open = not is_open
        logging.info(message)
        return jsonify(message), 200

    except Exception as e:
        error_message = {"error": str(e)}
        logging.error(error_message)
        return jsonify(error_message), 500

if __name__ == '__main__':
    # Jalankan server pada port 5000 dan dapat diakses oleh seluruh perangkat di dalam jaringan
    logging.info("Starting the server...")
    app.run(host='0.0.0.0', port=5000)
