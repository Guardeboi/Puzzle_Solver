/**
 * @file puzzle.cpp
 * Implementation of puzzle class.
 */
#include "puzzle.h"
#include <limits>
#include <map>
#include <deque>
#include <unordered_map>
#include <queue>
#include <set>

PuzzleState::PuzzleState() {
  for (int i = 0; i < 15; i++) {
    arr[i] = i + 1;
}
arr[15] = 0;
}

PuzzleState::PuzzleState(const std::array<char, 16> state) {
  arr = state;
}

bool PuzzleState::operator==(const PuzzleState &rhs) const {
  return arr == rhs.arr;
}

std::array<char, 16> PuzzleState::asArray() const {
  return arr;
}

bool PuzzleState::operator!=(const PuzzleState &rhs) const {
  return !(arr == rhs.arr);
}

bool PuzzleState::operator<(const PuzzleState &rhs) const {
  for (int i = 0; i < 16; i++) {
    if (arr[i] != rhs.arr[i]) {
      return arr[i] < rhs.arr[i];
    }
  }
  return false;
}

int PuzzleState::zero() const {
  for (int i = 0; i < 16; i++) {
    if (arr[i] == 0) {
      return i;
    }
  }
  return std::numeric_limits<int>::min();
}

PuzzleState PuzzleState::getNeighbor(Direction direction) const {
  int fzero = zero();
  if (fzero < 0 || fzero == 16) {
    std::array<char, 16> empty{0};
    for (int i = 0; i < 16; i++) {
      empty[i] = 0; 
    }
    return PuzzleState(empty);
  }
  int zerorow = fzero/4;
  int zerocol = fzero % 4;

  if (direction == Direction::UP) {
    zerorow++;
  } else if (direction == Direction::DOWN) {
    zerorow--;
  } else if (direction == Direction::LEFT) {
    zerocol++;
  } else {
    zerocol--;
  }

  if (zerorow < 0 || zerorow == 4 || zerocol < 0 || zerocol == 4) {
    std::array<char, 16> empty{0};
    for (int i = 0; i < 16; i++) {
      empty[i] = 0; 
    }
    return PuzzleState(empty);
  }

  int i = zerorow * 4 + zerocol;
  std::array<char, 16> copy = arr;
  std::swap(copy[i], copy[fzero]);
  return PuzzleState(copy);
}

std::vector<PuzzleState> PuzzleState::getNeighbors() const {
  std::vector<PuzzleState> vect;
  PuzzleState up = getNeighbor(Direction::UP);
  PuzzleState down = getNeighbor(Direction::DOWN);
  PuzzleState left = getNeighbor(Direction::LEFT);
  PuzzleState right = getNeighbor(Direction::RIGHT);
  std::array<char, 16> empty{};
  for (int i = 0; i < 16; i++) {
    empty[i] = 0; 
  }
  if (up.arr != empty) {
    vect.push_back(up);
  } 
  if (down.arr != empty) {
    vect.push_back(down);
  } 
  if (left.arr != empty) {
    vect.push_back(left);
  }
  if (right.arr != empty) {
    vect.push_back(right);
  }

  return vect;
}

int PuzzleState::manhattanDistance(const PuzzleState desiredState) const {
  int num = 0;
  std::array<char, 16> tmp_2 = desiredState.asArray();
  std::array<char, 16> tmp_1 = this->asArray();
  int maps[16] = {0};
  int index = 0;
  for (char i : tmp_2) {
    if (i != 0) {
      maps[(int) i] = index;
    } 
    index++;
  }
  index = 0;
  for (char i : tmp_1) {
    if (i == 0) {
      index++;
      continue;
    }
    int n = maps[(int) i];
    int col_1 = n % 4;
    int row_1 = n / 4;

    int row_2 = index / 4;
    int col_2 = index % 4;
    int rowdist = std::abs(row_1-row_2);
    int coldist = std::abs(col_1-col_2);
    num = num + rowdist + coldist;
    index++;
  }

  return num;
}

std::vector<PuzzleState> Path_Tracing(std::map<PuzzleState, PuzzleState> &map, PuzzleState curr) {
  std::vector<PuzzleState> arr;

  std::deque<PuzzleState> dq;
  dq.push_front(curr);

  while (map.find(curr) != map.end()) {
    curr = map[curr];
    dq.push_front(curr);
  }

  while (!dq.empty()) {
    arr.push_back(dq.front());
    dq.pop_front();
  }
  return arr;
}

/**
* Solves the puzzle using BFS.
* @param startState The starting state of the puzzle
* @param desiredState The final goal state of the puzzle after solving
* @param iterations The number of iterations it took to solve the puzzle. An
* iteration is defined as the number of times a state is popped from the data
* structure (NOTE: this should include the start and desired states, but not any
* states that are immediately discarded, if applicable). We will use the value
* stored at this pointer to evaluate efficiency. Ignore if NULL.
* @return The path to the solution. The first element of the vector is the start
* state, and the last element is the desired state. Empty if no solution exists.
*/

std::vector<PuzzleState> solveBFS(const PuzzleState &startState, const PuzzleState &desiredState, size_t *it) {
  std::vector<PuzzleState> ans;
  if (it != NULL) {
    (*it) = 0;
  }

  if (startState == desiredState) {
    ans.push_back(desiredState);
    (*it) = 1;
    return ans;
  }

  std::queue<PuzzleState> q;

  std::set<PuzzleState> visited;

  std::map<PuzzleState, PuzzleState> m;

  q.push(startState);

  visited.insert(startState);

  while (!q.empty()) {
    PuzzleState tmp = q.front();
    q.pop();

    if (it) {
      (*it)++;
    }

    if (tmp == desiredState) {
      return Path_Tracing(m, tmp);
    }
    std::vector<PuzzleState> neigh = tmp.getNeighbors();
    for (PuzzleState i : neigh) {
      if (visited.count(i)) {
        continue;
      } else {
        visited.insert(i);
        m[i] = tmp;
        q.push(i);
      }
    }
  }
  return ans;
}

/*
Pseudocode
The following pseudocode describes the algorithm:

function reconstruct_path(cameFrom, current)
    total_path := {current}
    while current in cameFrom.Keys:
        current := cameFrom[current]
        total_path.prepend(current) - use deque for this
    return total_path

// A* finds a path from start to goal.
// h is the heuristic function. h(n) estimates the cost to reach goal from node n.
function A_Star(start, goal, h)
    // The set of discovered nodes that may need to be (re-)expanded.
    // Initially, only the start node is known.
    // This is usually implemented as a min-heap or priority queue rather than a hash-set. 
    openSet := {start}

    // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from the start
    // to n currently known.
    cameFrom := an empty map

    // For node n, gScore[n] is the currently known cost of the cheapest path from start to n.
    gScore := map with default value of Infinity
    gScore[start] := 0

    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
    // how cheap a path could be from start to finish if it goes through n.
    fScore := map with default value of Infinity
    fScore[start] := h(start)

    while openSet is not empty
        // This operation can occur in O(Log(N)) time if openSet is a min-heap or a priority queue
        current := the node in openSet having the lowest fScore[] value
        if current = goal
            return reconstruct_path(cameFrom, current)

        openSet.Remove(current)
        for each neighbor of current
            // d(current,neighbor) is the weight of the edge from current to neighbor
            // tentative_gScore is the distance from start to the neighbor through current
            tentative_gScore := gScore[current] + d(current, neighbor)
            if tentative_gScore < gScore[neighbor]
                // This path to neighbor is better than any previous one. Record it!
                cameFrom[neighbor] := current
                gScore[neighbor] := tentative_gScore
                fScore[neighbor] := tentative_gScore + h(neighbor)
                if neighbor not in openSet
                    openSet.add(neighbor)

    // Open set is empty but goal was never reached
    return failure
*/

std::vector<PuzzleState> solveAstar(const PuzzleState& startState, const PuzzleState &desiredState, size_t *it) {
  std::vector<PuzzleState> arr;

  if (it) {
    *it = 0;
  }
  
  std::priority_queue<fandg, std::vector<fandg>, std::greater<fandg>> pq;
  int md = startState.manhattanDistance(desiredState);
  pq.push({md, 0, startState});

  std::map<PuzzleState, int> m;

  std::map<PuzzleState, PuzzleState> prev;

  std::set<PuzzleState> visited;

  m[startState] = 0;

  while (!pq.empty()) {
    fandg tmp = pq.top();
    pq.pop();

    if (m.count(tmp.puzz) == true && tmp.gscore != m[tmp.puzz]) {
      continue;
    } else if (visited.count(tmp.puzz) == true) {
      continue;
    }

    if (it != NULL) {
      (*it)++;
    }

    if (tmp.puzz == desiredState) {
      return Path_Tracing(prev, tmp.puzz);
    }

    visited.insert(tmp.puzz);

    std::vector<PuzzleState> neigh = tmp.puzz.getNeighbors();
   
    for (PuzzleState  i : neigh) {
      if (visited.count(i)) {
        continue;
      }
      int neighbor_dist = 1;
      int score = tmp.gscore + neighbor_dist;

      if (m.count(i) == false || score < m[i]) {
        prev[i] = tmp.puzz;
        m[i] = score;
        int f = score + i.manhattanDistance(desiredState);
        pq.push({f, score, i});
      }
    }
  }
  return {};
}
