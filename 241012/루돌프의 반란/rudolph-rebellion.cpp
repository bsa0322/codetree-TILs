#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
typedef pair<int, int> pi;
const int INF = 2e9;

// 상, 우, 하, 좌, 좌상, 우상, 우하, 좌하
int dr[8] = {-1, 0, 1, 0, -1, -1, 1, 1};
int dc[8] = {0, 1, 0, -1, -1, 1, 1, -1};

// 산타 최종 점수
vector<int> ans;
vector<bool> is_die; // 탈락 여부
vector<int> is_active; // 기절 여부 (0: 활동 가능, 1 이상 : 기절한 턴 수)


int calcDist(int r1, int c1, int r2, int c2){
    return pow(r1 - r2, 2) + pow(c1 - c2, 2);
}

bool checkBound(int &n, int &r, int &c){
    return ((r >= 0) && (r < n) && (c >= 0) && (c < n));
}

int oppositeDir(int dir){
    if (dir % 4 <= 1){
        return dir + 2;
    }else{
        return dir - 2;
    }
}

void printBoard(vector<vector<int>> &board){
    for(int i=0;i<board.size();i++){
        for(int j=0;j<board[0].size();j++){
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
    cout<<"=======================\n";
    return;
}

// 루돌프와 가장 가까운 산타 구하기
int nearSantaFromR(int &n, int &p, vector<pi> &init_pos){
    int rr = init_pos[0].first, rc = init_pos[0].second;
    int sn = 0;
    int dist = INF;
    for(int i=1;i<=p;i++){
        if(is_die[i]) continue; // 죽은 산타라면 넘어감
        int temp_dist = calcDist(rr, rc, init_pos[i].first, init_pos[i].second);
        if (dist > temp_dist){
            dist = temp_dist;
            sn = i;
        }else if(dist == temp_dist){
            if (init_pos[i].first > init_pos[sn].first){
                sn = i;
            }else if(init_pos[i].first == init_pos[sn].first && init_pos[i].second > init_pos[sn].second){
                sn = i;
            }
        }
    }
    return sn;
}

// 충돌로 인한 산타 움직이기 (dir 방향으로 움직이기)
void moveBackSanta(int cur_sn, int r, int c, int dir, int &n, int &p, vector<pi> &init_pos, vector<vector<int>> &board){
    while(checkBound(n, r, c) && board[r][c] > 0 && board[r][c] <= p){
        int new_sn = board[r][c];
        board[r][c] = cur_sn;
        init_pos[cur_sn] = {r, c};
        r += dr[dir];
        c += dc[dir];
        cur_sn = new_sn;
    }
    if(!checkBound(n, r, c)){
        // 범위 벗어났다면
        is_die[cur_sn] = true;
    }else{
        board[r][c] = cur_sn;
        init_pos[cur_sn] = {r, c};
    }

    return;
}

// 루돌프 움직이기
void moveR(int idx, int &sn, int &n, int &p, int &c, vector<pi> &init_pos, vector<vector<int>> &board){
    int dir = -1; // 가장 가까워지는 방향
    int dist = INF;
    // 가장 가까워지는 방향 구하기
    for(int i=0;i<8;i++){
        int nr = init_pos[0].first + dr[i];
        int nc = init_pos[0].second + dc[i];
        if (!checkBound(n, nr, nc)){
            continue;
        }
        int temp_dist = calcDist(nr, nc, init_pos[sn].first, init_pos[sn].second);
        if(dist > temp_dist){
            dist = temp_dist;
            dir = i;
        }
    }
    
    // 루돌프 움직이기
    int nrr = init_pos[0].first + dr[dir];
    int nrc = init_pos[0].second + dc[dir];

    // 루돌프 & 산타 충돌한다면 -> c 만큼 밀려남 !!
    if(board[nrr][nrc] > 0 && board[nrr][nrc] <= p){
        // 점수 증가
        ans[sn] += c;
        is_active[sn] = idx;
        int sr = nrr + dr[dir] * c;
        int sc = nrc + dc[dir] * c;

        if(!checkBound(n, sr, sc)){
            // 바로 죽음
            is_die[board[nrr][nrc]] = true;
        }else if (board[sr][sc] > 0 && board[sr][sc] <= p) {
            // 산타가 있다면 -> 산타 밀려나기 (충돌)
            moveBackSanta(sn, sr, sc, dir, n, p, init_pos, board);
        }else{
            // 밀려난 곳에 산타 없다면
            board[sr][sc] = sn;
            init_pos[sn] = {sr, sc};
        }
    }
    board[init_pos[0].first][init_pos[0].second] = 0;
    init_pos[0] = {nrr, nrc};
    board[nrr][nrc] = p + 1;
    return;
}

// 산타 움직이기
void moveS(int idx, int sn, int &n, int &p, int &d, vector<pi> &init_pos, vector<vector<int>> &board){
    int dir = -1; // 가장 가까워지는 방향
    int dist = INF;
    int origin_dist = calcDist(init_pos[sn].first, init_pos[sn].second, init_pos[0].first, init_pos[0].second);
    // 가장 가까워지는 방향 구하기
    for(int i=0;i<4;i++){
        int nr = init_pos[sn].first + dr[i];
        int nc = init_pos[sn].second + dc[i];
        if (!checkBound(n, nr, nc) || (board[nr][nc] > 0 && board[nr][nc] <= p)){
            // 범위 벗어나거나 산타 이미 있다면 넘어감
            continue;
        }
        int temp_dist = calcDist(nr, nc, init_pos[0].first, init_pos[0].second);
        if(origin_dist > temp_dist && dist > temp_dist){
            dist = temp_dist;
            dir = i;
        }
    }
    if(dir == -1){
        return;
    }

    // 산타 움직이기
    int nrr = init_pos[sn].first + dr[dir];
    int nrc = init_pos[sn].second + dc[dir];
    board[init_pos[sn].first][init_pos[sn].second] = 0;

    // 루돌프 & 산타 충돌한다면 -> d 만큼 밀려남 !!
    if(board[nrr][nrc] == (p + 1)){
        // 점수 증가
        ans[sn] += d;
        is_active[sn] = idx;
        int sr = nrr - dr[dir] * d;
        int sc = nrc - dc[dir] * d;

        if(!checkBound(n, sr, sc)){
            // 바로 죽음
            is_die[sn] = true;
        }else if (board[sr][sc] > 0 && board[sr][sc] <= p) {
            // 산타가 있다면 -> 산타 밀려나기 (충돌)
            moveBackSanta(sn, sr, sc, oppositeDir(dir), n, p, init_pos, board);
        }else{
            // 밀려난 곳에 산타 없다면
            board[sr][sc] = sn;
            init_pos[sn] = {sr, sc};
        }
    }else{
        init_pos[sn] = {nrr, nrc};
        board[nrr][nrc] = sn;
    }
    return;
}

void solution(int &n, int &m, int &p, int &c, int &d, vector<pi> &init_pos, vector<vector<int>> &board){
    // 게임 턴 수 만큼
    for(int i=1;i<=m;i++){
        // 기절한 산타 있으면 깨우기
        for(int j=1;j<=p;j++){
            if((is_active[j] > 0) && ((is_active[j] + 2) == i)){
                is_active[j] = 0;
            }
        }
        // 루돌프 움직이기
        int near_sn = nearSantaFromR(n, p, init_pos); // 가장 가까운 산타
        moveR(i, near_sn, n, p, c, init_pos, board); // 루돌프 이동

        // (디버깅용)
        // cout<<i<< " 번째 루돌프 이동 후\n";
        // printBoard(board);

        // 산타 움직이기
        for(int j=1;j<=p;j++){
            if(is_active[j] == 0 && !is_die[j]){
                moveS(i, j, n, p, d, init_pos, board);
            }
        }

        // (디버깅용)
        // cout<<i<< " 번째 산타 이동 후\n";
        // printBoard(board);
        
        bool flag = true;
        for(int j=1;j<=p;j++){
            if(!is_die[j]){
                // 아직 죽지 않았다면
                flag = false;
                ans[j] += 1;
            }
        }
        if (flag){
            break;
        }
    }
    return;
}

int main() {
    int n, m, p, c, d, rr, rc, pn, sr, sc;
    cin >> n >> m >> p >> c >> d >> rr >> rc;

    vector<pi> init_pos(p + 1);
    // p + 1: 루돌프, 1 ~ p: 산타, -(1 ~ p): 기절 산타
    vector<vector<int>> board(n, vector<int>(n,0));
    ans.assign(p + 1, 0);
    is_active.assign(p + 1, 0);
    is_die.assign(p + 1, false);

    init_pos[0] = {rr - 1, rc - 1}; // 루돌프 위치
    for(int i = 0; i < p; i++){
        cin >> pn >> sr >> sc;
        board[sr-1][sc-1] = pn; // 산타 위치
        init_pos[pn] = {sr - 1, sc - 1};
    }
    board[rr-1][rc-1] = p + 1; //루돌프 위치

    // 산타 최종 점수
    solution(n, m, p, c, d, init_pos, board);

    for(int i=1;i<=p;i++){
        cout << ans[i] << ' ';
    }

    return 0;
}