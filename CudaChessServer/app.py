from flask import Flask, render_template
from flask_socketio import SocketIO, emit

app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True
socketio = SocketIO(app)

values = {}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/board/<path:data>')
def board(data):
    values['board'] = data
    socketio.emit('update board', data)
    return "data sent"

@app.route('/get/<path>')
def get(data):
    if data in values: return data[values]
    return "null value"

@socketio.on('connect')
def socker_connect():
    if 'board' in values:
        emit('update board', values['board'])

@socketio.on('update move')
def socket_update_move(message):
    values[message['path']] = message['value']
    print(values)

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0')