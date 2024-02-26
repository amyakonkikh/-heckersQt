#include "gameboard.h"

GameBoard::GameBoard(QObject *parent) : QAbstractTableModel(parent)
{
    initBoard();
}

int GameBoard::rowCount(const QModelIndex &) const
{
    return m_demension;
}

int GameBoard::columnCount(const QModelIndex &) const
{
    return m_demension;
}

QVariant GameBoard::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid())
      return QVariant();

    switch (role)
    {
        case VisibleRole :
          return QVariant(m_Checkers[index.row()][index.column()].visible);

        case CheckerColorRole:
          return QVariant(m_Checkers[index.row()][index.column()].player == numPlayer::firstPlayer ?
                          colorFirst : colorSecond);

        case BoardColorRole :
          return QVariant(m_Checkers[index.row()][index.column()].isMoving == true ?
                          QBrush(Qt::red) : QBrush(Qt::transparent));

        case MouseEnableRole :
          return QVariant(m_Checkers[index.row()][index.column()].isMoving);

        case KingRole :
          return QVariant(m_Checkers[index.row()][index.column()].isKing);

        default :
          break;
    }

    return QVariant();
}

QHash<int, QByteArray> GameBoard::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles[VisibleRole] = "visibl";
    roles[CheckerColorRole] = "checkerColor";
    roles[BoardColorRole] = "boardColor";
    roles[MouseEnableRole] = "mouseEnable";
    roles[KingRole] = "king";

    return roles;
}

void GameBoard::restartGame()
{
    m_Checkers.clear();
    initBoard();
    firstCheckers = 12;
    secondCheckers = 12;
    currentPlayer = numPlayer::firstPlayer;
}


void GameBoard::move()
{
    const int rowFrom = m_indexMoveFrom % m_demension;
    const int colFrom = m_indexMoveFrom / m_demension;
    const int rowTo = m_indexMoveTo % m_demension;
    const int colTo = m_indexMoveTo / m_demension;

    if (m_Checkers[rowFrom][colFrom].isMoving && m_Checkers[rowTo][colTo].isMoving &&
       (rowFrom != rowTo && colFrom != colTo) && WillEat)
    {
        scoring();
        deleteChecker(rowFrom, colFrom, rowTo, colTo);
        std::swap(m_Checkers[rowFrom][colFrom], m_Checkers[rowTo][colTo]);
        clearBoard(rowFrom, colFrom);
        becomeKing(rowTo, colTo);

        if (EatAgain(rowTo, colTo) && WillEat)
        {
            Continue = true;
            m_indexMoveFrom = m_indexMoveTo;
            m_Checkers[rowTo][colTo].isMoving = true;
            qDebug() << "Can eat again";
        }

        else
        {
            Continue = false;
            m_Checkers[rowTo][colTo].isMoving = false;
        }

        if (!Continue)
          switchPlayer();
    }

    else if (m_Checkers[rowFrom][colFrom].isMoving && m_Checkers[rowTo][colTo].isMoving &&
            (rowFrom != rowTo && colFrom != colTo) && !WillEat)
    {
        std::swap(m_Checkers[rowFrom][colFrom], m_Checkers[rowTo][colTo]);
        clearBoard(rowFrom, colFrom);
        becomeKing(rowTo, colTo);
        switchPlayer();
    }

    emit dataChanged(createIndex(0,0), createIndex(63, 63));
}

void GameBoard::firstColorChange()
{
    colorFirst = QBrush(Qt::white);
    colorSecond = QBrush(Qt::black);

    emit dataChanged(createIndex(0,0), createIndex(63, 63));
}

void GameBoard::secondColorChange()
{
    colorFirst = QBrush(Qt::black);
    colorSecond = QBrush(Qt::white);

    emit dataChanged(createIndex(0,0), createIndex(63, 63));
}

void GameBoard::checkerSelection()
{
    const int rowFrom = m_indexMoveFrom % m_demension;
    const int colFrom = m_indexMoveFrom / m_demension;

    if (m_Checkers[rowFrom][colFrom].player == currentPlayer)
    {
        WillEat = IsCheckerHaveEatStep(rowFrom, colFrom, !m_Checkers[rowFrom][colFrom].isKing, Continue);
        qDebug() << QStringLiteral("Checker Have Eat Step") << WillEat;

        emit dataChanged(createIndex(0, 0), createIndex(63, 63));
    }
}

void GameBoard::scoring()
{
    if (currentPlayer == numPlayer::firstPlayer)
    {
        --secondCheckers;

        if (secondCheckers == 0)
        {
            ++firstWin;
            emit firstWinChanged(firstWin);
            emit gameEndWindow();
        }
    }

    else if (currentPlayer == numPlayer::secondPlayer)
    {
        --firstCheckers;

        if (firstCheckers == 0)
        {
            ++secondWin;
            emit secondWinChanged(secondWin);
            emit gameEndWindow();
        }
    }
}

void GameBoard::initBoard()
{
    m_Checkers.resize(m_demension, QVector<Checker>(m_demension, Checker()));

    for (int row = 0; row < m_demension; ++row)
    {
        for (int col = 0; col < m_demension; ++col)
        {
            if (((col + row) % 2 == 1) && ((row >= 0 && row < 3) || (row > 4 && row <= 7)))
            {
                m_Checkers[row][col].visible = true;

                if (row < 3)
                  m_Checkers[row][col].player = numPlayer::secondPlayer;

                else if (row > 4)
                  m_Checkers[row][col].player = numPlayer::firstPlayer;
            }
        }
    }
}

bool GameBoard::EatAgain(const int row, const int col)
{
    return IsCheckerHaveEatStep(row, col, !m_Checkers[row][col].isKing, true);
}

void GameBoard::clearBoard(const int rowFrom, const int colFrom)
{
    for (int row = 0; row < m_demension; ++row)
    {
        for (int col = 0; col < m_demension; ++col)
        {
            if (m_Checkers[row][col].isMoving)
            {
                m_Checkers[row][col].isMoving = false;
                m_Checkers[row][col].wantToEat = false;
                m_Checkers[rowFrom][colFrom].wantToDelete.clear();
            }
        }
    }
}


void GameBoard::forEatChecker(const int row, const int col, const int _i, const int _j, const int a, const int b)
{
    m_Checkers[_i + a][_j + b].isMoving = true;
    m_Checkers[row][col].wantToDelete[ {_i + a, _j + b} ] = {_i, _j};
    m_Checkers[row][col].wantToEat = true;
}

void GameBoard::forNotEatting(const int _i, const int _j)
{
    m_Checkers[_i][_j].isMoving = true;
    cancelNotEat.push_back( {_i, _j} );
}

void GameBoard::forKingEat(const int row, const int col, const int _i, const int _j, const std::pair<int, int> &delet)
{
    m_Checkers[_i][_j].isMoving = true;
    m_Checkers[row][col].wantToDelete[ {_i, _j} ] = delet;
}

bool GameBoard::up_right(const int row, const int col, const bool isOneStep, const bool isContinue)
{
    std::pair<int, int> checkerToDelete = {};
    bool eatStep = false;
    int j = col + 1;

    for (int i = row - 1; i >= 0; --i)
    {
        if (currentPlayer == numPlayer::secondPlayer && isOneStep && !isContinue)
          break;

        if (isValidIndex(i, j))
        {
            if (m_Checkers[i][j].player != numPlayer::noPlayer && m_Checkers[i][j].player != currentPlayer)
            {

                if (!isValidIndex(i - 1, j + 1) || (m_Checkers[i - 1][j + 1].player != numPlayer::noPlayer))
                {
                    if (!isOneStep)
                      break;

                    eatStep = false;
                }

                else
                {
                    eatStep = true;
                    forEatChecker(row, col, i, j, -1, 1);

                    if (!isOneStep)
                      checkerToDelete = {i, j};
                }
            }

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && !m_Checkers[row][col].wantToEat && !isContinue)
              forNotEatting(i,j);

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && eatStep && !isOneStep)
              forKingEat(row, col, i, j, checkerToDelete);
        }

        if (isValidIndex(i, j) && m_Checkers[i][j].player == currentPlayer)
          break;

        if (j < 7)
          ++j;

        else
          break;

        if (isOneStep)
          break;
    }

    return eatStep;
}

bool GameBoard::up_left(const int row, const int col, const bool isOneStep, const  bool isContinue)
{
    std::pair<int, int> checkerToDelete = {};
    bool eatStep = false;
    int j = col - 1;

    for (int i = row - 1; i >= 0; --i)
    {
        if (currentPlayer == numPlayer::secondPlayer && isOneStep && !isContinue)
          break;

        if (isValidIndex(i, j))
        {
            if (m_Checkers[i][j].player != numPlayer::noPlayer && m_Checkers[i][j].player != currentPlayer && m_Checkers[i][j].visible)
            {
                if (!isValidIndex(i - 1, j - 1) || (m_Checkers[i - 1][j - 1].player != numPlayer::noPlayer))
                {
                    if (!isOneStep)
                      break;

                    eatStep = false;
                }

                else
                {
                    eatStep = true;
                    forEatChecker(row, col, i, j, -1, -1);

                    if (!isOneStep)
                      checkerToDelete = {i, j};
                }
            }

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && !m_Checkers[row][col].wantToEat && !isContinue)
              forNotEatting(i,j);

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && eatStep && !isOneStep)
              forKingEat(row, col, i, j, checkerToDelete);
        }

        if (isValidIndex(i, j) && m_Checkers[i][j].player == currentPlayer)
          break;

        if (j > 0)
          --j;

        else
          break;

        if (isOneStep)
          break;
    }

    return eatStep;
}

bool GameBoard::down_right(const int row, const int col, const bool isOneStep, const bool isContinue)
{
    std::pair<int, int> checkerToDelete = {};
    bool eatStep = false;
    int j = col + 1;

    for (int i = row + 1; i < 8; ++i)
    {
        if (currentPlayer == numPlayer::firstPlayer && isOneStep && !isContinue)
          break;

        if (isValidIndex(i, j))
        {
            if (m_Checkers[i][j].player != numPlayer::noPlayer && m_Checkers[i][j].player != currentPlayer && m_Checkers[i][j].visible)
            {
                if (!isValidIndex(i + 1, j + 1) || (m_Checkers[i + 1][j + 1].player != numPlayer::noPlayer))
                {
                    if (!isOneStep)
                      break;

                    eatStep = false;
                }

                else
                {
                    eatStep = true;
                    forEatChecker(row, col, i, j, 1, 1);

                    if (!isOneStep)
                      checkerToDelete = {i, j};
                }
            }

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && !m_Checkers[row][col].wantToEat && !isContinue)
              forNotEatting(i,j);

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && eatStep && !isOneStep)
              forKingEat(row, col, i, j, checkerToDelete);
        }

        if (isValidIndex(i, j) && m_Checkers[i][j].player == currentPlayer)
          break;

        if (j < 7)
          ++j;

        else
          break;

        if (isOneStep)
          break;
    }

    return eatStep;
}

bool GameBoard::down_left(const int row, const int col, const bool isOneStep, const bool isContinue)
{
    std::pair<int, int> checkerToDelete = {};
    bool eatStep = false;
    int j = col - 1;

    for (int i = row + 1; i < 8; ++i)
    {
        if (currentPlayer == numPlayer::firstPlayer && isOneStep && !isContinue)
          break;

        if (isValidIndex(i, j))
        {
            if (m_Checkers[i][j].player != numPlayer::noPlayer && m_Checkers[i][j].player != currentPlayer && m_Checkers[i][j].visible)
            {

                if (!isValidIndex(i + 1, j - 1) || (m_Checkers[i + 1][j - 1].player != numPlayer::noPlayer))
                {
                    if (!isOneStep)
                      break;

                    eatStep = false;
                }

                else
                {
                    eatStep = true;
                    forEatChecker(row, col, i, j, 1, -1);

                    if (!isOneStep)
                      checkerToDelete = {i, j};
                }

            }

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && !m_Checkers[row][col].wantToEat && !isContinue)
              forNotEatting(i,j);

            else if (m_Checkers[i][j].player == numPlayer::noPlayer && eatStep && !isOneStep)
              forKingEat(row, col, i, j, checkerToDelete);
        }

        if (isValidIndex(i, j) && m_Checkers[i][j].player == currentPlayer)
          break;

        if (j > 0)
          --j;

        else
          break;

        if (isOneStep)
          break;
    }

    return eatStep;
}

bool GameBoard::IsCheckerHaveEatStep(const int row, const int col, const bool isOneStep, const bool isContinue)
{
    bool result = false;
    const bool ur = up_right(row, col, isOneStep, isContinue);   // ленивость условного оператора не подходит
    const bool ul = up_left(row, col, isOneStep, isContinue);    // нужно проверить все напрвления
    const bool dr = down_right(row, col, isOneStep, isContinue);
    const bool dl = down_left(row, col, isOneStep, isContinue);

    if (ur || ul || dr || dl)
    {
        for (const auto &i : cancelNotEat)
          m_Checkers[i.first][i.second].isMoving = false;

        cancelNotEat.clear();
        result = true;
    }

    m_Checkers[row][col].isMoving = true;

    return result;
}

int GameBoard::demension() const
{
    return m_demension;
}

void GameBoard::switchPlayer()
{
    currentPlayer = currentPlayer == numPlayer::firstPlayer ?
                numPlayer::secondPlayer : numPlayer::firstPlayer;
}

bool GameBoard::isValidIndex(const int row, const int col) const
{
    if (row >= m_demension || row < 0 || col >= m_demension || col < 0)
      return false;

    return true;
}

void GameBoard::deleteChecker(const int rowFrom, const int colFrom, const int rowTo, const int colTo)
{
    const int rowDel = m_Checkers[rowFrom][colFrom].wantToDelete[ {rowTo, colTo} ].first;
    const int colDel = m_Checkers[rowFrom][colFrom].wantToDelete[ {rowTo, colTo} ].second;
    m_Checkers[rowDel][colDel].visible = false;
    m_Checkers[rowDel][colDel].player = numPlayer::noPlayer;
}

void GameBoard::becomeKing(const int row, const int col)
{
    if (m_Checkers[row][col].player == numPlayer::firstPlayer && row == 0)
      m_Checkers[row][col].isKing = true;

    else if (m_Checkers[row][col].player == numPlayer::secondPlayer && row == 7)
        m_Checkers[row][col].isKing = true;
}
