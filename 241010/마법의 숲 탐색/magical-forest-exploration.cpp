#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;
typedef pair<int, int> pi;
const int DS = 4; // 방향 개수

int n, m, k;
vector<vector<int>> board; // 0: 아무것도 없음, 양수: 골렘, 음수: 출구
int dr[5] = {-1, 0, 1, 0, 0}; // 북, 동, 남, 서, 제자리(더미)
int dc[5] = {0, 1, 0, -1, 0}; // 북, 동, 남, 서, 제자리(더미)

bool chkGoDirec(int row, int col, vector<pi> &direc){
    // cout << "크기: "<<direc.size()<<'\n';
    for(int i=0;i<direc.size();i++){
        // cout<<direc[i].first<<' '<<direc[i].second<<'\n';
        int nr = row + direc[i].first;
        int nc = col + direc[i].second;

        if(nr < -2 || nr > n || nc < 1 || nc > m){
            return false;
        }
        if(nr > 0 && board[nr][nc] != 0){
            return false;
        }
    }
    return true;
}

/**
 * 골렘 움직이기 (숲 탐색)
 * return 골렘 움직인 후 정령 위치
 */
 pi moveGolem(int idx, int col, int dir){
    int row = -1;
    vector<pi> direc_south = {{1, -1}, {2, 0}, {1, 1}}; // 남쪽
    vector<pi> direc_west = {{-1, -1}, {0, -2}, {1, -2}, {1, -1}, {2, -1}}; // 서쪽
    vector<pi> direc_east = {{-1, 1}, {0, 2}, {1, 1}, {1, 2}, {2, 1}}; // 동쪽

    while (true){
        // cout << "현재 위치: "<<row<<", "<<col<<'\n';
        // 남쪽으로 갈 수 있는지 확인
        if (chkGoDirec(row, col, direc_south)){
            row += 1;
            continue;
        }

        // 서쪽으로 갈 수 있는지 확인
        if (chkGoDirec(row, col, direc_west)){
            row += 1;
            col -= 1;
            dir = (dir + 3) % DS;
            continue;
        }

        // 동쪽으로 갈 수 있는지 확인
        if (chkGoDirec(row, col, direc_east)){
            row += 1;
            col += 1;
            dir = (dir + 1) % DS;
            continue;
        }

        break;
    }

    // 골렘 몸 일부가 벗어났는지 확인
    if (row < 2){
        return {row, col};
    }

    // 벗어나지 않았다면 -> 골렘 board 에 표시
    for (int i=0;i<5;i++){
        int nr = row + dr[i];
        int nc = col + dc[i];
        board[nr][nc] = idx;
    }
    board[row + dr[dir]][col + dc[dir]] = -idx; // 출구
    
    return {row, col};
 }

 /**
  * 정령 남쪽으로 이동시키기 (BFS 탐색)
  * return 정령의 최종 행 위치
  */
int moveTinkerbell(pi pos){
    queue <pi> q;
    vector<vector<bool>> check(n+1, vector<bool>(m+1, false));

    q.push(pos);
    check[pos.first][pos.second] = true;
    int answer = 0;
    while(!q.empty()){
        int cr = q.front().first;
        int cc = q.front().second;
        q.pop();

        answer = max(answer, cr);
        for(int i=0;i<DS;i++){
            int nr = cr + dr[i];
            int nc = cc + dc[i];
            // 범위 벗어남
            if(nr < 1 || nr > n || nc < 1 || nc > m){
                continue;
            }
            // 이미 체크했다면
            if(check[nr][nc]){
                continue;
            }
            // 골렘 없음
            if(board[nr][nc] == 0){
                continue;
            }
            // 현재 위치가 출구가 아닌데 다른 골렘으로 가려는 경우
            if(board[cr][cc] > 0 && board[cr][cc] != abs(board[nr][nc])){
                continue;
            }
            q.push({nr, nc});
            check[nr][nc] = true;
        }
    }

    return answer;
}

int main() {
    int golem_col, golem_dir, ans = 0;

    cin >> n >> m >> k;
    board.assign(n+1, vector<int>(m+1, 0));

    for(int i=0;i<k;i++){
        cin >> golem_col >> golem_dir;
        
        // 골렘 움직이기 (숲 탐색)
        pi pos = moveGolem(i + 1, golem_col, golem_dir);

        // cout<<"현재 board 상태\n";
        // for(int j=1;j<=n;j++){
        //     for(int l=1;l<=m;l++){
        //         cout<<board[j][l]<<' ';
        //     }
        //     cout<<'\n';
        // }
        // cout<<pos.first<<' '<<pos.second<<'\n';

        // 골렘의 몸이 벗어났다면 -> board 초기화
        if (pos.first < 2){
            board.assign(n+1, vector<int>(m+1, 0));
            continue;
        }

        // 정령 남쪽으로 움직이기 
        int last_row = moveTinkerbell(pos);

        // cout << "최종 행 위치: "<<last_row<<'\n';

        // 정답 더하기
        ans += last_row;
    }

    cout << ans;

    return 0;
}