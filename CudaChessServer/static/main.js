var board;
var sounds;

class Sound{
    playSound(url) {
        var ourAudio = document.createElement('audio'); // Create a audio element using the DOM
        ourAudio.style.display = "none"; // Hide the audio element
        ourAudio.src = url; // Set resource to our URL
        ourAudio.autoplay = true; // Automatically play sound
        ourAudio.onended = function() {
          this.remove(); // Remove when played.
        };
        document.body.appendChild(ourAudio);
    }
    move(){
        this.playSound("/static/sounds/move.mp3");
    }
    capture(){
        this.playSound("/static/sounds/capture.mp3");
    }
}



class ChessBoard {
    constructor() {
        this.isMouseDown = false;
        this.array = new Array(8).fill(" ").map(() => new Array(8).fill(" "));

        this.chessCanvas = $("#chess_canvas")
        this.ctx = this.chessCanvas[0].getContext("2d");
        this.scale = Math.min(this.chessCanvas.width(), this.chessCanvas.height()) / 8;
        
        this.drawBoard();

        var names = ["black-king","black-queen","black-rook","black-bishop","black-knight","black-pawn","white-king","white-queen","white-rook","white-bishop", "white-knight",  "white-pawn"]
        var letters = ["k", "q", "r", "b", "n", "p", "K", "Q", "R", "B", "N", "P"]

        this.images = {}

        for(var i = 0; i < names.length; i++){
            this.images[letters[i]] = document.getElementById(names[i]);
        }
        

        this.selectedX = -1;
        this.selectedY = -1;

        this.chessCanvas.on('mousedown', this.mousedown);
        this.chessCanvas.on('mousemove', this.mousemove);
        this.chessCanvas.on('mouseup', this.mouseup);
    }

    drawFEN(s) {
        this.array = new Array(8).fill(" ").map(() => new Array(8).fill(" "));

        var y = 7;
        var x = 0;

        for(var i = 0; i < s.indexOf(" "); i++){
            var c = s.charAt(i);
            if(c == "/"){
                y -= 1;
                x = 0;
            }else if(c >= '0' && c <= '9'){
                x += parseInt(c);
            }else{
                this.array[y][x] = c
                x += 1;
            }
        }

        this.drawArray();
    }

    drawArray(){
        this.drawBoard();

        for(var x = 0; x < 8; x++){
            for(var y = 0; y < 8; y++){
                if(this.array[y][x] in this.images && !(x == this.selectedX && y == this.selectedY)){
                    this.drawPiece(this.array[y][x], x, y);
                }
            }
        }
    }

    drawBoard(l, x, y) {
        for (let x = 0; x < 8; x++) {
            for (let y = 0; y < 8; y++) {
                this.drawBoardAtPoint(x, y, (x == this.selectedX && y == this.selectedY))
            }
        }
    }

    selectPoint(ax, ay) {
        var x = Math.floor(ax / board.scale);
        var y = 7 - Math.floor(ay / board.scale);
        this.selectedX = x;
        this.selectedY = y;

        this.drawArray();
        this.drawPieceFloating(this.array[y][x], ax - this.scale / 2, ay - this.scale / 2);
    }

    moveSelected(ax, ay){
        this.drawArray();
        this.drawPieceFloating(this.array[this.selectedY][this.selectedX], ax - this.scale / 2, ay - this.scale / 2);
    }

    unselectPoint(ax, ay) {
        var x = this.selectedX, y = this.selectedY;

        board.selectedX = -1;
        board.selectedY = -1;

        this.drawArray();
    }

    drawBoardAtPoint(x, y, highlight = false){
        this.ctx.fillStyle = ((x + y) % 2 == 0) ? ((!highlight) ? "#996633" : "#bbbbbb") : ((!highlight) ? "#bb8844" : "#bbbbbb");
        this.ctx.fillRect(x * this.scale, (7 - y) * this.scale, this.scale, this.scale);
    }

    drawPiece(l, x, y) {
        this.drawPieceFloating(l, x * this.scale, (7 - y) * this.scale);
    }

    drawPieceFloating(l, ax, ay) {
        if(l in this.images){
            this.ctx.drawImage(this.images[l], ax, ay, this.scale, this.scale);
        }
    }

    

    mousedown(e){
        board.isMouseDown = true;
        var x = e.offsetX, y = e.offsetY;

        board.selectPoint(x, y);

        sounds.move();
    }
    mousemove(e){
        if(board.isMouseDown){
            var x = e.offsetX, y = e.offsetY;

            board.moveSelected(x, y);
        }
    }
    mouseup(e){
        board.isMouseDown = false;
        var x = e.offsetX, y = e.offsetY;
        
        board.unselectPoint(x, y);

        sounds.capture();
    }
}


$(document).ready(function () {
    setTimeout(function () {
        sounds = new Sound();
        board = new ChessBoard();

        var socket = io.connect('http://localhost:5000');

        socket.on('update board', function (msg) {
            msg = msg.replaceAll('_', ' ');
            console.log('update board', msg);
            board.drawFEN(msg);
        });
    }, 200);
});