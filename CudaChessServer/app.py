from flask import Flask, render_template
from flask_socketio import SocketIO, emit

app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True
socketio = SocketIO(app)

values = {"board": "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR_w_KQkq_-_0_1"}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/set/<id>/<path:data>')
def setValue(id, data):
    values[id] = data
    socketio.emit(f'update {id}', data)
    return "data sent"

@app.route('/get/<id>')
def getValue(id):
    if id in values: return values[id]
    return "" # Default value

@socketio.on('connect')
def socker_connect():
    for id in values:
        emit(f'update {id}', values[id])

@socketio.on('set')
def socket_update_move(message):
    values[message['id']] = message['data']
    print(values)

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0')