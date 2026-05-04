#include "GameStateFileHandler.h"

#include <fstream>
#include <limits>

bool GameStateFileHandler::saveGameState(const std::string& filePath, const GameSnapshot& snapshot)
{
    std::ofstream file(filePath);

    if (!file)
        return false;

    file << "CurrentPuzzleIndex: " << snapshot.getCurrentPuzzleIndex() << std::endl;
    file << "PuzzleCount: " << snapshot.getBoardCount() << std::endl;

    for (int puzzleIndex = 0; puzzleIndex < snapshot.getBoardCount(); puzzleIndex++)
    {
        file << "PuzzleIndex: " << puzzleIndex << std::endl;
        file << "Board:" << std::endl;

        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                file << snapshot.getValue(puzzleIndex, row, col);

                if (col < 7)
                    file << " ";
            }
            file << std::endl;
        }
        
        file << "Solved: " << (snapshot.getSolved(puzzleIndex) ? 1 : 0) << std::endl;
        file << "Seconds: " << snapshot.getSeconds(puzzleIndex) << std::endl;
    }

    return true;
}

bool GameStateFileHandler::loadGameState(const std::string& filePath, GameSnapshot& snapshot)
{
    std::ifstream file(filePath);

    if (!file)
        return false;

    std::string line;

    std::getline(file, line);
    int currentPuzzleIndex = std::stoi(line.substr(line.find(":") + 1));

    std::getline(file, line);
    int boardCount = std::stoi(line.substr(line.find(":") + 1));

    if (boardCount != snapshot.getBoardCount())
        return false;

    if (currentPuzzleIndex < 0 || currentPuzzleIndex >= boardCount)
        return false;

    snapshot.setCurrentPuzzleIndex(currentPuzzleIndex);

    for (int i = 0; i < boardCount; i++)
    {
        std::getline(file, line);
        int puzzleIndex = std::stoi(line.substr(line.find(":") + 1));

        std::getline(file, line);

        if (puzzleIndex < 0 || puzzleIndex >= boardCount)
            return false;

        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                int value;
                if (!(file >> value))
                    return false;

                snapshot.setValue(puzzleIndex, row, col, value);
            }
        }
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        std::getline(file, line);
        int solvedFlag = std::stoi(line.substr(line.find(":") + 1));
        snapshot.setSolved(puzzleIndex, solvedFlag == 1);

        std::getline(file, line);
        int seconds = std::stoi(line.substr(line.find(":") + 1));
        snapshot.setSeconds(puzzleIndex, seconds);
    }

    return true;
}