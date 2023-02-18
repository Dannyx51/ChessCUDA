
// the cuda library
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// the chess library and its components
#include "chess_board.h"
#include <algorithm>

// the web stuff ;-;
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>

void sendFEN(ChessBoard board);
void sendMoves(ChessBoard board);

int main()
{
    std::string test = "8/8/P2P2P1/P1PqP1P1/P1P1P1P1/P2P2P1/PP4PP/8 w - - 0 1";

    ChessBoard board = ChessBoard();

    std::vector<Move> moves = board.getMoves(10);

    std::cout << board.toFEN();

    std::cout << "\n{";
    for(Move m : moves) {
        std::cout << m << ", ";
    }
    std::cout << "}\n";

	sendFEN(board);

    return 0;
}

void sendFEN(ChessBoard board) {
    std::string fen = board.toFEN();
    std::replace(fen.begin(), fen.end(), ' ', '_');

    std::string url = "https://localhost:5000";
    
    Poco::URI uri(url);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    
    session.setKeepAlive(true);

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/board/" + fen, Poco::Net::HTTPMessage::HTTP_1_1);

    session.sendRequest(request);

    Poco::Net::HTTPResponse response;
    std::istream& page = session.receiveResponse(response);

    std::cout << "HTTP Status: " << response.getStatus() << "\n";
    
    session.setKeepAlive(false);
}

void sendMoves(ChessBoard board) {
    std::string url = "https://localhost:5000";

    Poco::URI uri(url);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
}