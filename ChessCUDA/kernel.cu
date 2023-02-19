
// the cuda library
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// the chess library and its components
#include "chess_board.h"
#include <algorithm>
#include <unordered_map>

// the web stuff ;-;
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>


std::unordered_map<std::string, std::vector<Move>> calculateAllMoves(ChessBoard board);

void sendFEN(ChessBoard board);
void sendMoves(std::unordered_map<std::string, std::vector<Move>> moves);
Move getPlayed();

int main()
{
    ChessBoard board = ChessBoard();

	sendFEN(board);

    auto allMoves = calculateAllMoves(board);

    sendMoves(allMoves);

	Move move = getPlayed();

    if(move.isValid()){
        board.makeMove(move);

		std::cout << move << std::endl;
        std::cout << board << std::endl;

        sendFEN(board);
    }

    return 0;
}

std::unordered_map<std::string, std::vector<Move>> calculateAllMoves(ChessBoard board) {

    std::unordered_map<std::string, std::vector<Move>> allMoves;

    for (int i = 0; i < 64; ++i) {
        if (board.board[i] != Piece::NONE) {
            std::string loc = std::string(1, (i % 8) + 'a') + std::to_string(i / 8 + 1);

            allMoves[loc] = board.getMoves(i);
        }
    }

    return allMoves;
}


void sendFEN(ChessBoard board) {
    std::string fen = board.toFEN();
    std::replace(fen.begin(), fen.end(), ' ', '_');

    std::string url = "https://localhost:5000";
    
    Poco::URI uri(url);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    
    session.setKeepAlive(true);

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/set/board/" + fen, Poco::Net::HTTPMessage::HTTP_1_1);

    session.sendRequest(request);

    Poco::Net::HTTPResponse response;
    std::istream& page = session.receiveResponse(response);

    std::cout << "SEND FEN\t- HTTP Status: " << response.getStatus() << "\n";
    
    session.setKeepAlive(false);
}

void sendMoves(std::unordered_map<std::string, std::vector<Move>> const moves) {

    std::string url = "https://localhost:5000";

    std::string parsedMoves = "";

    for (auto const& piece : moves) {
        parsedMoves += piece.first;
        for (Move move : piece.second) {
            parsedMoves += move.to_string(true);
        }
        parsedMoves += "_";
    }

    Poco::URI uri(url);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    session.setKeepAlive(true);

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/set/moves/" + parsedMoves, Poco::Net::HTTPMessage::HTTP_1_1);
    
    session.sendRequest(request);

    Poco::Net::HTTPResponse response;
    std::istream& page = session.receiveResponse(response);

    std::cout << "SEND MOVES\t- HTTP Status: " << response.getStatus() << "\n";

    session.setKeepAlive(false);
}

Move getPlayed() {

	std::string url = "https://localhost:5000";

	Poco::URI uri(url);
	Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

	session.setKeepAlive(true);

	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/get/play", Poco::Net::HTTPMessage::HTTP_1_1);

	session.sendRequest(request);

	Poco::Net::HTTPResponse response;
	std::istream& page = session.receiveResponse(response);

	std::cout << "GET PLAYED\t- HTTP Status: " << response.getStatus() << "\n";

    std::string str(std::istreambuf_iterator<char>(page), {});

	session.setKeepAlive(false);

    return Move(str);
}

