var board;
var sounds;
var socket;


function posToCoord(s){
    var x = s.charCodeAt(0) - "a".charCodeAt(0);
    var y = s.charCodeAt(1) - "1".charCodeAt(0);

    return [x, y]
}

function coordToPos(x, y) {
    return "abcdefgh".charAt(x) + (y + 1).toString()
}


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
        
        var names = ["black-king","black-queen","black-rook","black-bishop","black-knight","black-pawn","white-king","white-queen","white-rook","white-bishop", "white-knight",  "white-pawn"]
        var letters = ["k", "q", "r", "b", "n", "p", "K", "Q", "R", "B", "N", "P"]

        this.images = {}

        for(var i = 0; i < names.length; i++){
            this.images[letters[i]] = document.getElementById(names[i]);
        }
        

        this.selectedX = -1;
        this.selectedY = -1;

        this.moves = {"c2": ["c3", "c4"]};
        

        this.chessCanvas.on('mousedown', this.mousedown);
        this.chessCanvas.on('mousemove', this.mousemove);
        this.chessCanvas.on('mouseup', this.mouseup);
        this.chessCanvas.on('mouseout', this.mouseout);

        this.drawBoard();
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
    drawBoard() {
        var moveTargets = [];
        if(this.selectedX != -1 && this.selectedY != -1){
            var pos = coordToPos(this.selectedX, this.selectedY);

            if(pos in this.moves){
                moveTargets = this.moves[pos];
            }
        }

        for (let x = 0; x < 8; x++) {
            for (let y = 0; y < 8; y++) {
                this.drawBoardAtPoint(x, y, (x == this.selectedX && y == this.selectedY), moveTargets.includes(coordToPos(x, y)));
            }
        }
    }
    drawBoardAtPoint(x, y, highlight = false, moveTarget = false){
        if(highlight) {
            this.ctx.fillStyle = "#ddddaa";
        }else if(moveTarget) {
            this.ctx.fillStyle = ((x + y) % 2 == 0) ? "#999977" : "#aaaa88";
        }else{
            this.ctx.fillStyle = ((x + y) % 2 == 0) ? "#996633" : "#bb8844" ;
        }
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



    selectPoint(ax, ay) {
        var x = Math.floor(ax / board.scale);
        var y = 7 - Math.floor(ay / board.scale);

        if(this.array[y][x] != " "){
            this.selectedX = x;
            this.selectedY = y;

            this.drawArray();
            this.drawPieceFloating(this.array[y][x], ax - this.scale / 2, ay - this.scale / 2);

            sounds.move();
        }
    }
    moveSelected(ax, ay){
        if(this.selectedX != -1 && this.selectedY != -1){
            this.drawArray();

            this.drawPieceFloating(this.array[this.selectedY][this.selectedX], ax - this.scale / 2, ay - this.scale / 2);
        }
    }
    unselectPoint(ax, ay) {
        if(this.selectedX != -1 && this.selectedY != -1){ 
            var sx = this.selectedX, sy = this.selectedY;

            board.selectedX = -1;
            board.selectedY = -1;

            var x = Math.floor(ax / board.scale);
            var y = 7 - Math.floor(ay / board.scale);

            var fromLoc = coordToPos(sx, sy);
            var toLoc = coordToPos(x, y);
            
            if(fromLoc in this.moves && this.moves[fromLoc].includes(toLoc)){
                socket.emit("set", {"id": "play", "data": fromLoc + "-" + toLoc});
            }else{
                this.drawArray();

                sounds.capture();
            }

        }
    }
    cancelSelect(ax, ay) {
        if(this.selectedX != -1 && this.selectedY != -1){ 
            board.selectedX = -1;
            board.selectedY = -1;

            this.drawArray();

            sounds.capture();
        }
    }



    mousedown(e){
        board.isMouseDown = true;

        board.selectPoint(e.offsetX, e.offsetY);
    }
    mousemove(e){
        if(board.isMouseDown){
            board.moveSelected(e.offsetX, e.offsetY);
        }
    }
    mouseup(e){
        board.isMouseDown = false;

        board.unselectPoint(e.offsetX, e.offsetY);
    }
    mouseout(e){
        board.isMouseDown = false;

        board.cancelSelect(e.offsetX, e.offsetY);
    }
}


$(document).ready(function () {
    setTimeout(function () {
        sounds = new Sound();
        board = new ChessBoard();

        socket = io.connect('http://localhost:5000');

        socket.on('update board', function (msg) {
            msg = msg.replaceAll('_', ' ');
            console.log("update board: ", msg);
            board.drawFEN(msg);
        });

        socket.on('update moves', function(msg){
            data = msg.split("_");

            moves = {}

            for(var k = 0; k < data.length; k++) {
                var arr = [];

                for(var i = 2; i < data[k].length; i++){
                    arr.push(data[k].substring(i, i + 2));
                }

                moves[data[k].substring(0, 2)] = arr;
            }

            board.moves = moves;

            console.log(moves);
        });
    }, 200);
});