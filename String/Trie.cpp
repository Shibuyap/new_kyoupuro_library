/************************************************************
  Trie (Prefix Tree)  –  文字列集合を辺長 O(L) で操作
  ----------------------------------------------------------
  ◆ できること (L = 文字列長)        | 時間計算量
    - 文字列の挿入 insert(s)        | O(L)
    - 完全一致判定 contains(s)      | O(L)
    - 接頭辞出現回数 prefix_count(p) | O(L)
    - 辞書順列挙 enumerate()        | O(∑|s|)
    - 削除 erase(s) *optional       | O(L)
    - 最短一意接頭辞 min_unique(s)   | O(L)
    - 2 字句の共通接頭辞 LCP(s,t)    | O(min(|s|,|t|))

  ◆ 主要構造体
      struct Trie {
          void insert(const string&);
          bool contains(const string&) const;
          int  prefix_count(const string&) const;
          // 追加: erase / enumerate / min_unique など
      }

  ◆ 典型用法
      Trie tr;
      for(auto& w: dict) tr.insert(w);
      if(tr.contains("apple")) ...
      int num = tr.prefix_count("app");   // "app*" に何語？
      string mup = tr.min_unique("banana"); // "banan" など
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

/*------------------------------------------------------------
  Trie (lower-case a-z 固定版)
    * 26 文字限定で配列遷移を使い超高速
    * 汎用版 (map/unordered_map) が欲しければ
      child を array<int,σ>→unordered_map<char,int> に置換
------------------------------------------------------------*/
struct Trie
{
  static constexpr int ALPHA = 26; // ‘a’-‘z’
  struct Node
  {
    int next[ALPHA];
    int term = 0; // 完全一致個数 (multiset 用)
    int cnt = 0;  // この節点以下の文字列総数
    Node() { fill(begin(next), end(next), -1); }
  };
  vector<Node> tr;
  Trie() { tr.emplace_back(); }

  inline int idx(char c) const { return c - 'a'; }

  // 挿入: 文字列は 'a'-'z'
  void insert(const string &s)
  {
    int v = 0;
    for (char ch : s)
    {
      int c = idx(ch);
      if (tr[v].next[c] == -1)
      {
        tr[v].next[c] = (int)tr.size();
        tr.emplace_back();
      }
      v = tr[v].next[c];
      ++tr[v].cnt;
    }
    ++tr[v].term;
  }

  // 完全一致判定
  bool contains(const string &s) const
  {
    int v = 0;
    for (char ch : s)
    {
      int c = idx(ch);
      if (tr[v].next[c] == -1)
        return false;
      v = tr[v].next[c];
    }
    return tr[v].term > 0;
  }

  // 接頭辞 p を持つ語の個数
  int prefix_count(const string &p) const
  {
    int v = 0;
    for (char ch : p)
    {
      int c = idx(ch);
      if (tr[v].next[c] == -1)
        return 0;
      v = tr[v].next[c];
    }
    return tr[v].cnt;
  }

  // 最短一意接頭辞を返す（語が一意でなければ ""）
  string min_unique(const string &s) const
  {
    int v = 0;
    string res;
    for (char ch : s)
    {
      int c = idx(ch);
      if (tr[v].next[c] == -1)
        return ""; // 未登録
      v = tr[v].next[c];
      res += ch;
      if (tr[v].cnt == tr[v].term)
        return res; // 以降はこの語のみ
    }
    return (tr[v].term == 1 ? res : "");
  }

  // DFS で辞書順列挙（大きな辞書では注意）
  vector<string> enumerate() const
  {
    vector<string> out, buf;
    string tmp;
    dfs(0, tmp, out);
    return out;
  }

private:
  void dfs(int v, string &cur, vector<string> &out) const
  {
    if (tr[v].term)
      out.push_back(cur);
    for (int c = 0; c < ALPHA; ++c)
    {
      int nxt = tr[v].next[c];
      if (nxt != -1)
      {
        cur.push_back(char('a' + c));
        dfs(nxt, cur, out);
        cur.pop_back();
      }
    }
  }
};

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  Trie tr;
  tr.insert("apple");
  tr.insert("app");
  tr.insert("apply");
  tr.insert("banana");

  cout << boolalpha << tr.contains("app") << "\n"; // true
  cout << tr.prefix_count("app") << "\n";          // 3
  cout << tr.min_unique("banana") << "\n";         // "banan"

  auto list = tr.enumerate();
  for (auto &w : list)
    cout << w << " "; // app apple apply banana
  cout << "\n";
}
