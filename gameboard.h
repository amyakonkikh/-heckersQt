#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QAbstractTableModel>
#include <QVector>
#include <QBrush>
#include <QDebug>
#include <QVector>

enum class numPlayer
{
    noPlayer,
    firstPlayer,
    secondPlayer
};

struct Checker
{
    using checkersToDelete = std::map<std::pair<int,int>,std::pair<int,int>>;
    bool isKing = false;
    bool visible = false;
    bool isMoving = false;
    bool wantToEat = false;
    numPlayer player = numPlayer::noPlayer;
    checkersToDelete wantToDelete;
};

enum Roles {
    VisibleRole = Qt::UserRole + 1,
    CheckerColorRole,
    BoardColorRole,
    MouseEnableRole,
    KingRole
};

class GameBoard : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int demension READ demension CONSTANT)
    Q_PROPERTY(int indexMoveTo MEMBER m_indexMoveTo NOTIFY indexMoveToChanged)
    Q_PROPERTY(int indexMoveFrom MEMBER m_indexMoveFrom NOTIFY indexMoveFromChanged)
    Q_PROPERTY(int firstWin MEMBER firstWin NOTIFY firstWinChanged)
    Q_PROPERTY(int secondWin MEMBER secondWin NOTIFY secondWinChanged)

public:
    GameBoard(QObject *parent = nullptr);

    int demension() const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, const int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void restartGame();
    Q_INVOKABLE void move();
    Q_INVOKABLE void firstColorChange();
    Q_INVOKABLE void secondColorChange();
    Q_INVOKABLE void checkerSelection();


signals:
    void indexMoveFromChanged(int);
    void indexMoveToChanged(int);
    void firstWinChanged(int);
    void secondWinChanged(int);
    void gameEndWindow();

private:
    void scoring();
    void initBoard();
    void switchPlayer();
    bool EatAgain(const int row, const int col);
    void becomeKing(const int row, const int col);
    void forNotEatting(const int _i, const int _j);
    bool isValidIndex(const int row, const int col) const;
    void clearBoard(const int rowFrom, const int colFrom);
    bool up_left(const int row, const int col, const bool isOneStep, const bool isContinue);
    bool up_right(const int row, const int col, const bool isOneStep, const bool isContinue);
    bool down_left(const int row, const int col, const bool isOneStep, const bool isContinue);
    bool down_right(const int row, const int col, const bool isOneStep, const bool isContinue);
    void deleteChecker(const int rowFrom, const int colFrom, const int rowTo, const int colTo);
    bool IsCheckerHaveEatStep(const int row, const int col, const bool isOneStep, const bool isContinue);
    void forEatChecker(const int row, const int col, const int _i, const int _j, const int a, const int b);
    void forKingEat(const int row, const int col, const int _i, const int _j, const std::pair<int, int> &delet);

    int firstWin = 0;
    int secondWin = 0;
    int firstCheckers = 12;
    int secondCheckers = 12;
    bool WillEat = false;
    bool Continue = false; // возможность продолжения серии рубки
    int m_indexMoveTo = 0;
    int m_indexMoveFrom = 0;
    const int m_demension = 8;
    QBrush colorFirst = QBrush(Qt::transparent);
    QBrush colorSecond = QBrush(Qt::transparent);
    QVector<QVector<Checker>> m_Checkers;
    QVector<std::pair<int, int>> cancelNotEat;
    numPlayer currentPlayer = numPlayer::firstPlayer;
};

#endif // GAMEBOARD_H
