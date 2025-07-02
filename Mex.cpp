#include <bits/stdc++.h>
using namespace std;

/*-----------------------------------------------------------
  MexTracker
    配列長 N が与えられた後、値の挿入・削除を O(log N) で行い、
    常に現在の mex を O(1) で取得できる軽量クラス。
-----------------------------------------------------------*/
class MexTracker
{
public:
  explicit MexTracker(int n) : LIM(n + 1), cnt(LIM, 0)
  {
    for (int v = 0; v < LIM; ++v)
      S.insert(v); // 最初は 0..LIM-1 全て欠けている
  }

  // 値 v を 1 個追加
  void add(long long v)
  {
    if (v < LIM && cnt[v]++ == 0)
      S.erase((int)v); // 初めて登場 → 候補集合から除去
  }

  // 値 v を 1 個削除
  void remove(long long v)
  {
    if (v < LIM && --cnt[v] == 0)
      S.insert((int)v); // 個数が 0 になった → 候補集合に復活
  }

  // 現在の mex を取得
  int mex() const { return *S.begin(); }

private:
  const int LIM;   // 監視対象 = 0..N
  vector<int> cnt; // cnt[v] : 値 v の出現個数
  set<int> S;      // cnt[v]==0 の v を保持（mex = *S.begin()）
};

/*-----------------------------------------------------------
  ABC330 E  – Mex and Update
  元コードを MexTracker を用いた実装に書き換え
-----------------------------------------------------------*/
int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int N, Q;
  if (!(cin >> N >> Q))
    return 0;
  vector<long long> A(N);
  for (auto &a : A)
    cin >> a;

  MexTracker mt(N);
  for (auto v : A)
    mt.add(v); // 初期配列を反映

  while (Q--)
  {
    int i;
    long long x;
    cin >> i >> x;
    --i;             // 0-index へ
    mt.remove(A[i]); // 旧値を取り除く
    A[i] = x;
    mt.add(x);                // 新値を反映
    cout << mt.mex() << '\n'; // 現在の mex
  }
}
