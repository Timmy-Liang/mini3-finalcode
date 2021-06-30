#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <algorithm>

struct Point {
	int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player, noshow=1;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> boardarr;
std::vector<Point> next_vspots;

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
  //  bool done;
  //  int winner;

    OthelloBoard() {
        //reset();
    }
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> cpy, int player, std::vector<Point> cpy2) {
        cur_player = player;
        for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) {
            board[i][j] = cpy[i][j];
            //if (board[i][j] == WHITE) disc_count[WHITE]++;
            if (board[i][j] == WHITE) disc_count[WHITE]++;
            else if (board[i][j] == BLACK) disc_count[BLACK]++;
            else disc_count[EMPTY]++;
        }
        int size_cpy = cpy2.size();
        for (int i = 0; i < size_cpy; i++)
            next_valid_spots.push_back(cpy2[i]);
    }
    void equ(OthelloBoard cpy) {
        cur_player = cpy.cur_player;
        for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) {
            board[i][j] = cpy.board[i][j];
            /*if (board[i][j] == WHITE) disc_count[WHITE]++;
            else if (board[i][j] == BLACK) disc_count[BLACK]++;
            else disc_count[EMPTY]++;*/
        }
        disc_count[WHITE] = cpy.disc_count[WHITE];
        disc_count[BLACK] = cpy.disc_count[BLACK];
        disc_count[EMPTY] = cpy.disc_count[EMPTY];
    }
    /*
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> b,int player){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j]=b[i][j];
                if(board[i][j]==BLACK)disc_count[BLACK]++;
                else if(board[i][j]==WHITE)disc_count[WHITE]++;
                else disc_count[EMPTY]++;
            }
        }
        cur_player=player;
        next_valid_spots = get_valid_spots();
        done=false;
        winner=-1;

    }*/
    
    OthelloBoard(const OthelloBoard &boa){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j]=boa.board[i][j];
            }
        }
        disc_count[EMPTY]=boa.disc_count[EMPTY];
        disc_count[BLACK]=boa.disc_count[BLACK];
        disc_count[WHITE]=boa.disc_count[WHITE];
        cur_player=boa.cur_player;
        next_valid_spots=boa.next_valid_spots;
        //done=boa.done;
        //winner=boa.winner;
    }
    /*
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }*/

private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    public:
        std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        /*
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }*/
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        
        return true;
    }
};
void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> boardarr[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_vspots.push_back({x, y});
    }
}



bool incorner(Point a){
    if(a.x==0||a.x==7){
        if(a.y==0||a.y==7)return 1;
    }
    return 0;
}


int alphabeta(const OthelloBoard &board,int depth,int alpha,int beta,int pl,int first,std::ofstream& fout){
    //change
    if(depth==0){
        return board.disc_count[player]-board.disc_count[3-player];
    }
    for(int i=0;i<3;i++){
        if(board.disc_count[i]==0){
            return board.disc_count[player]-board.disc_count[3-player];
        }
    }
    
    if(pl==player){
        int value=-100000;
        int bsize=board.next_valid_spots.size();
        for(int i=0;i<bsize;i++){
            OthelloBoard tb(board);
            //change

            tb.put_disc(board.next_valid_spots[i]);
            int tv=alphabeta(tb,depth-1,alpha,beta,tb.cur_player,0,fout);
            if(first){
                if(tv>value||noshow){
                    fout<<board.next_valid_spots[i].x << " " <<board.next_valid_spots[i].y<< std::endl;
                    fout.flush();
                    //change
                    noshow=0;
                }
            }
            //change
            value=std::max(value,tv);
            alpha=std::max(value,alpha);
            if(alpha>=beta)break;
        }
            return value;
    }
    else {
        int bsize=board.next_valid_spots.size();
        int value=100000;
            for(int i=0;i<bsize;i++){
                if(incorner(board.next_valid_spots[i])){
                    value=-100000;
                    beta=-100000;
                    return value;
                }
                OthelloBoard tb(board);
                
                tb.put_disc(board.next_valid_spots[i]);
                value=std::min(value,alphabeta(tb,depth-1,alpha,beta,tb.cur_player,0,fout));
                beta=std::min(value,beta);
                if(beta<=alpha)break;
            }
            return value;
    }
}



void write_valid_spot(std::ofstream& fout) {
//set
/*
    if(n_valid_spots!=0){
        pp=next_vspots[0];
        fout << pp.x << " " << pp.y << std::endl;
        fout.flush();
    } */
    OthelloBoard curboard(boardarr,player,next_vspots);  
    alphabeta(curboard,6,-100000,100000,player,1,fout);
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
