#include "chess_board.h"

ChessBoard::ChessBoard() {
	board[0] = ROOK | WHITE;
	board[1] = KNIGHT | WHITE;
	board[2] = BISHOP | WHITE;
	board[3] = QUEEN | WHITE;
	board[4] = KING | WHITE;
	board[5] = BISHOP | WHITE;
	board[6] = KNIGHT | WHITE;
	board[7] = ROOK | WHITE;
	for (int i = 8; i < 16; ++i) {
		board[i] = PAWN | WHITE;
	}

	for (int i = 48; i < 56; ++i) {
		board[i] = PAWN | BLACK;
	}

	board[56] = ROOK | BLACK;
	board[57] = KNIGHT | BLACK;
	board[58] = BISHOP | BLACK;
	board[59] = QUEEN | BLACK;
	board[60] = KING | BLACK;
	board[61] = BISHOP | BLACK;
	board[62] = KNIGHT | BLACK;
	board[63] = ROOK | BLACK;

	for (int i = 0; i < 4; ++i) {
		castle[i] = 1;
	}

	computeStepsToEdge();
}

ChessBoard::ChessBoard(std::string FEN) {
	int y = 7, x = 0;

	int index = 0;

	uint8_t team;
	for (int i = 0; i < FEN.find(' '); ++i) {
		if (FEN[i] == '/') {
			y -= 1;
			x = 0;
			continue;
		}

		if (isdigit(FEN[i])) {
			x += FEN[i] - '0';
			continue;
		}

		team = (islower(FEN[i])) ? BLACK : WHITE;

		char piece = tolower(FEN[i]);

		switch (piece) {
		case 'p':
			board[y * 8 + x] = PAWN | team;
			break;
		case 'n':
			board[y * 8 + x] = KNIGHT | team;
			break;
		case 'b':
			board[y * 8 + x] = BISHOP | team;
			break;
		case 'r':
			board[y * 8 + x] = ROOK | team;
			break;
		case 'q':
			board[y * 8 + x] = QUEEN | team;
			break;
		case 'k':
			board[y * 8 + x] = KING | team;
			break;
		default:
			board[y * 8 + x] = NONE;
		}

		++x;
	}

	int nextSpace = FEN.find(' ');

	playerMove = FEN[nextSpace + 1] == 'b';

	nextSpace = FEN.find(' ', nextSpace + 1);

	for (int i = nextSpace + 1; i < FEN.find(' ', nextSpace + 1); ++i) {
		switch (FEN[i]) {
		case 'K':
			castle[0] = 1;
			break;
		case 'Q':
			castle[1] = 1;
			break;
		case 'k':
			castle[2] = 1;
			break;
		case 'q':
			castle[3] = 1;
			break;
		}
	}

	nextSpace = FEN.find(' ', nextSpace + 1);

	if (FEN[nextSpace + 1] != '-') {
		int x = FEN[nextSpace + 1] - 'a';
		int y = (playerMove) ? 2 : 5;

		enPassantTarget = y * 8 + x;
	}

	nextSpace = FEN.find(' ', nextSpace + 1);

	halfMove = std::stoi(FEN.substr(nextSpace + 1, FEN.find(' ', nextSpace + 1)));

	nextSpace = FEN.find(' ', nextSpace + 1);

	fullMove = std::stoi(FEN.substr(nextSpace + 1));

	computeStepsToEdge();
}

void ChessBoard::computeStepsToEdge() {
	int ind;
	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {

			ind = y * 8 + x;

			stepsToEdge[ind][0] = 7 - y;
			stepsToEdge[ind][1] = y;
			stepsToEdge[ind][2] = 7 - x;
			stepsToEdge[ind][3] = x;
			stepsToEdge[ind][4] = (stepsToEdge[ind][0] < stepsToEdge[ind][3]) ? stepsToEdge[ind][0] : stepsToEdge[ind][3];
			stepsToEdge[ind][5] = (stepsToEdge[ind][1] < stepsToEdge[ind][2]) ? stepsToEdge[ind][1] : stepsToEdge[ind][2];
			stepsToEdge[ind][6] = (stepsToEdge[ind][0] < stepsToEdge[ind][2]) ? stepsToEdge[ind][0] : stepsToEdge[ind][2];
			stepsToEdge[ind][7] = (stepsToEdge[ind][1] < stepsToEdge[ind][3]) ? stepsToEdge[ind][1] : stepsToEdge[ind][3];
		}
	}
}

std::vector<Move> ChessBoard::getMoves(int ind) {
	uint8_t piece = board[ind];

	std::vector<Move> moves;

	uint8_t team = piece & 0b11000000;
	piece &= 0b00111111;

	// sliding pieces
	if ((QUEEN | ROOK | BISHOP) & piece) {
		int start = (BISHOP & piece) ? 4 : 0;
		int end = (ROOK & piece) ? 4 : 8;

		for (int i = start; i < end; ++i) {
			for (int d = 1; d <= stepsToEdge[ind][i]; ++d) {
				int localInd = ind + dirOffsets[i] * d;

				if (board[localInd] != NONE && board[localInd] & team) {
					break;
				}

				moves.push_back(Move{ ind, localInd });

				if (board[localInd] != NONE && !(board[localInd] & team)) {
					break;
				}

			}
		}
	}
	else if ((KNIGHT)&piece) {
		const int deltas[][2] = { {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1} };

		int dx, dy;
		for (int i = 0; i < 8; i++) {
			dx = deltas[i][0] + ind % 8;
			dy = deltas[i][1] + ind / 8;

			if (dx < 0 || dx > 7) continue;
			if (dy < 0 || dy > 7) continue;

			int localInd = dy * 8 + dx;

			if (board[localInd] == NONE || !(board[localInd] & team)) {
				moves.push_back(Move{ ind, localInd });
			}
		}
	}
	else if ((KING)&piece) {
		for (int i = 0; i < 8; ++i) {
			int d = 1;
			if (d <= stepsToEdge[ind][i]) {
				int localInd = ind + dirOffsets[i] * d;

				if (board[localInd] != NONE && board[localInd] & team) {
					continue;
				}

				moves.push_back(Move{ ind, localInd });
			}
		}
	}
	else if ((PAWN)&piece) {
		int start_rank = (team & WHITE) ? 1 : 6;;
		int dir = (team & BLACK) ? -1 : 1;

		// move forward
		if (board[ind + 8 * dir] == NONE) {
			moves.push_back(Move{ ind, ind + 8 * dir });

			if (ind / 8 == start_rank && board[ind + 16 * dir] == NONE) {
				moves.push_back(Move{ ind, ind + 16 * dir });
			}
		}

		// Capture
		if ((ind % 8 > 0) && ((board[ind + 8 * dir + 1] != NONE && !(board[ind + 8 * dir + 1] & team)) || (ind + 8 * dir + 1 == enPassantTarget))) {
			moves.push_back(Move{ ind, ind + 8 * dir + 1 });
		}
		if ((ind % 8 < 7) && ((board[ind + 8 * dir - 1] != NONE && !(board[ind + 8 * dir - 1] & team)) || (ind + 8 * dir - 1 == enPassantTarget))) {
			moves.push_back(Move{ ind, ind + 8 * dir - 1 });
		}
	}


	return moves;
}




void ChessBoard::makeMove(Move move) {
	board[move.end] = board[move.start];
	board[move.start] = NONE;
}


uint8_t ChessBoard::getLoc(int x, int y) {
	if (x < 0 || x > 7) return -1;
	if (y < 0 || y > 7) return -1;

	return board[x + y * 8];
}

std::string ChessBoard::toFEN() {
	std::string fen = "";

	for (int y = 7; y > -1; --y) {
		int noneCount = 0;
		for (int x = 0; x < 8; ++x) {
			if (board[y * 8 + x] == NONE) {
				++noneCount;
			}
			else {
				if (noneCount) {
					fen += std::to_string(noneCount);
					noneCount = 0;
				}
				
				uint8_t piece = board[y * 8 + x];
				bool team = piece & WHITE; // one of the few cases where true == white
				piece &= 0b00111111;

				char rep;
				switch (piece) {
				case PAWN:
					rep = 'p';
					break;
				case KNIGHT:
					rep = 'n';
					break;
				case BISHOP:
					rep = 'b';
					break;
				case ROOK:
					rep = 'r';
					break;
				case QUEEN:
					rep = 'q';
					break;
				case KING:
					rep = 'k';
					break;
				default:
					rep = 'x';
				}

				fen += ((team) ? std::string(1, toupper(rep)) : std::string(1, rep));
			}
		}

		if (noneCount) {
			fen += std::to_string(noneCount);
			noneCount = 0;
		}
		
		if (y) fen += "/";
	}

	// player turn
	fen += " ";
	fen += (playerMove) ? "b" : "w";

	// castle perms
	fen += " ";
	int checkCastles = castle[0] + castle[1] + castle[2] + castle[3];
	if (checkCastles) {
		if (castle[0]) {
			fen += "K";
		}
		if (castle[1]) {
			fen += "Q";
		}
		if (castle[2]) {
			fen += "k";
		}
		if (castle[3]) {
			fen += "q";
		}
	}
	else {
		fen += "-";
	}

	//en passant
	fen += " ";
	if (enPassantTarget) {
		fen += std::string(1, (char)(enPassantTarget % 8) + 'a') + std::to_string(enPassantTarget / 8 + 1);
	}
	else {
		fen += "-";
	}

	// move counters
	fen += " ";
	fen += std::to_string(halfMove);
	fen += " ";
	fen += std::to_string(fullMove);

	return fen;
}

std::ostream& operator<<(std::ostream& os, ChessBoard const& board) {
	os << "-----------------\n";
	for (int y = 7; y > -1; --y) {
		os << "|";
		for (int x = 0; x < 8; ++x) {
			bool team = board.board[y * 8 + x] & BLACK;
			switch (board.board[y * 8 + x] & 0b00111111) {
			case PAWN:
				os << ((team) ? "p" : "P");
				break;
			case KNIGHT:
				os << ((team) ? "n" : "N");
				break;
			case BISHOP:
				os << ((team) ? "b" : "B");
				break;
			case ROOK:
				os << ((team) ? "r" : "R");
				break;
			case QUEEN:
				os << ((team) ? "q" : "Q");
				break;
			case KING:
				os << ((team) ? "k" : "K");
				break;
			default:
				os << " ";
				break;
			}
			os << "|";
		}
		os << "\n-----------------\n";
	}

	os << "\nPlayer Move: " << ((board.playerMove) ? "B" : "W") << "\n";
	os << "Castle Perms: " << ((board.castle[0]) ? "K" : "") << ((board.castle[1]) ? "Q" : "") << ((board.castle[2]) ? "k" : "") << ((board.castle[3]) ? "q" : "") << "\n";
	os << "En Passant Target: " << ((board.enPassantTarget) ? std::string(1, (char)(board.enPassantTarget % 8) + 'a') + std::to_string(board.enPassantTarget / 8 + 1) : "NONE") << "\n";
	os << "Half Move: " << board.halfMove << "\n";
	os << "Full Move: " << board.fullMove << "\n";

	return os;
}

/// MOVE FUNCTIONS ///
Move::Move(std::string move) {
	if(move.length() != 5){
		start = -1;
		end = -1;
	}else{
		start = move[0] - 'a' + (move[1] - '1') * 8;
		end = move[3] - 'a' + (move[4] - '1') * 8;
	}
}

bool Move::isValid() {
	return start >= 0 && end >= 0 && start < 64 && end < 64;
}

std::string Move::to_string(bool e) {
	if (e) {
		return std::string(1, (char)(end % 8) + 'a') + std::to_string(end / 8 + 1);
	}
	return std::string(1, (char)(start % 8) + 'a') + std::to_string(start / 8 + 1);
}

std::ostream& operator<<(std::ostream& os, Move const& move) {
	os << std::string(1, (char)(move.start % 8) + 'a') << std::to_string(move.start / 8 + 1) << "-" << std::string(1, (char)(move.end % 8) + 'a') << std::to_string(move.end / 8 + 1);
	return os;
}