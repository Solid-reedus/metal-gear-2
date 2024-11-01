#include "precomp.h"
#include "MapManager.h"
#include "EnemyManager.h"
#include "commonTypes.h"
#include "pickupManager.h"


#include <iostream>


MapManager::MapManager(int2 p_mapRoomSize, char* p_csvMapFile, char* p_csvCollisionFile, LevelAudioManager* p_LevelAudioManager, Surface* p_screen)
{
    m_mapRoomSize = p_mapRoomSize;
    rooms = new DymArr<RoomStruct>;
    rooms->size = m_mapRoomSize.x * m_mapRoomSize.y;
    rooms->data = new RoomStruct[rooms->size]; // Ensure this aligns with `size` value

    GetMapFromCsv(p_csvMapFile);
    GetMapDataFromCsv(p_csvCollisionFile);
    SetCollisionMap(p_csvCollisionFile);
    //GetEnemiesFromCsv(p_csvGuardinstructions);
    m_TileSheet = Tilesheet();
    m_ptrLevelAudioManager = p_LevelAudioManager;
    m_pickupManager = new PickupManager(this, p_screen);
    m_screen = p_screen;

}

MapManager::~MapManager()
{
    delete[] rooms->data;
    delete[] m_ptrMapArray;
}



void MapManager::Render()
{
    if (!m_TileSheet.Initialized())
    {
        printf("error:spritesheet isnt initialized properly, in MapManager::Render\n");
        return;
    }
    rooms->data[m_currentRoom.y * m_mapRoomSize.x + m_currentRoom.x].renderSurf->CopyTo(m_screen, MAP_RENDERING_START, MAP_RENDERING_START);
}

void MapManager::RenderSecondLayer(int2 p_playerPos, bool p_isCrouching)
{

    int index = m_currentRoom.y * m_mapRoomSize.x + m_currentRoom.x;

    if (rooms->data[index].crouchAreaCount < 1)
    {
        return;
    }

    for (int i = 0; i < rooms->data[index].crouchAreaCount; i++)
    {
        int2 pos = rooms->data[index].ptrArrayCrouchAreas[i];

        if (p_playerPos.y > pos.y && !p_isCrouching)
        {
            continue;
        }

        int x = ((m_currentRoom.x) * m_roomSize.x) + pos.x;
        int y = ((m_currentRoom.y) * m_roomSize.y) + pos.y;
        int tileData = m_ptrMapArray[(x + y * m_pitch)];
        m_TileSheet.Draw(pos * TILE_SIZE + MAP_RENDERING_START, { tileData % m_TileSheet.m_size.x , tileData / m_TileSheet.m_size.x });
    }
    for (int i = 0; i < rooms->data[index].colliderCount; i++)
    {
        int2 pos = rooms->data[index].ptrArrayColliders[i];

        if (p_playerPos.y > pos.y && !p_isCrouching)
        {
            continue;
        }

        int x = ((m_currentRoom.x) * m_roomSize.x) + pos.x;
        int y = ((m_currentRoom.y) * m_roomSize.y) + pos.y;
        int tileData = m_ptrMapArray[(x + y * m_pitch)];
        m_TileSheet.Draw(pos * TILE_SIZE + MAP_RENDERING_START, { tileData % m_TileSheet.m_size.x , tileData / m_TileSheet.m_size.x });
    }
}

void MapManager::InitTileSheet(int2 p_tilesheetSize, char* p_spritesheetImage, Surface* p_ptrScreen)
{
    m_TileSheet = Tilesheet(p_tilesheetSize, p_spritesheetImage, p_ptrScreen);

    for (int i = 0; i < m_mapRoomSize.x * m_mapRoomSize.y; i++)
    {
        Surface* newrenderSurf = new Surface(ROOM_SIZE_IN_PIXELS, ROOM_SIZE_IN_PIXELS);
        int2 roomIndex = { i % m_mapRoomSize.x , i / m_mapRoomSize.x };


        for (int j = 0; j < ROOM_SIZE; j++)
        {
            for (int k = 0; k < ROOM_SIZE; k++)
            {
                int x = ((roomIndex.x) * ROOM_SIZE) + j;
                int y = ((roomIndex.y) * ROOM_SIZE) + k;
                int tileData = m_ptrMapArray[(x + y * m_pitch)];
                Surface* usedSurf = m_TileSheet.GetSpecificTile({ tileData % m_TileSheet.m_size.x , tileData / m_TileSheet.m_size.x });
                usedSurf->CopyTo(newrenderSurf, (j * TILE_SIZE), (k * TILE_SIZE));
            }
        }


        rooms->data[i].renderSurf = newrenderSurf;
    }


}

void MapManager::SetNewRoom(int2 p_pos)
{
    if (p_pos.x > -1 && p_pos.y > -1 && p_pos.x < m_mapRoomSize.x && p_pos.y < m_mapRoomSize.y)
    {
        m_pickupManager->UpdateCurrentPickupAble();
        m_currentRoom = p_pos;
    }
}

void MapManager::IncrementToNewRoom(int2 p_pos)
{

    if ((m_currentRoom.x + p_pos.x) > -1 && (m_currentRoom.x + p_pos.x) < m_mapRoomSize.x &&
        (m_currentRoom.y + p_pos.y) > -1 && (m_currentRoom.y + p_pos.y) < m_mapRoomSize.y)
    {
        m_currentRoom += p_pos;
        m_pickupManager->UpdateCurrentPickupAble();
    }
}

int MapManager::GetLineCount(const char* s) const
{
    const char* p = s;
    int lines = 0;
    while (*p) if (*p++ == '\n') lines++;
    return lines;
}

void MapManager::SetCollisionMap(char* p_csvFile)
{
    int size = m_mapRoomSize.x * m_mapRoomSize.y;
    int* roomDataSize = new int[size] {0};      // colliders
    int* roomDataSize2 = new int[size] {0};     // crouch
    int* roomDataSize3 = new int[size] {0};     // teleport
    int* roomDataSize4 = new int[size] {0};     // visible crouch

    char* data = GetCstringFromFile(p_csvFile);
    size_t dataLength = strlen(data);

    int mapSize = 0;
    char currentNumber[10] = { 0 };
    int currentSize = 0;

    for (size_t i = 0; i <= dataLength; i++) {
        char ch = data[i];

        if (ch == ',' || ch == '\n' || ch == '\0') 
        {
            if (currentSize > 0) 
            {
                currentNumber[currentSize] = '\0';  // Null-terminate
                int value = CharToInt(currentNumber);

                int2 pos = { (mapSize % m_pitch) % ROOM_SIZE, (mapSize / m_pitch) % ROOM_SIZE };
                int2 mapPos = { (mapSize % m_pitch) / ROOM_SIZE, (mapSize / m_pitch) / ROOM_SIZE };
                int mapindex = mapPos.x + mapPos.y * m_mapRoomSize.x;

                if (mapindex >= 0 && mapindex < size) 
                {
                    switch (value) 
                    {
                        case markerValues::MARKER_COLLIDER:
                        {
                            roomDataSize[mapindex]++;
                            break;
                        }
                        case markerValues::MARKER_CROUCH_AREA:
                        {
                            roomDataSize2[mapindex]++;
                            break;
                        }
                        case markerValues::MARKER_TELEPORT_MARKER:
                        {
                            roomDataSize3[mapindex]++;
                            break;

                        }
                        case markerValues::MARKER_VISIBLE_CROUCH_AREA:
                        {
                            roomDataSize4[mapindex]++;
                            break;
                        }
                        default:
                        break;
                    }
                }
                mapSize++;
                currentSize = 0; 
            }
        }
        else if (currentSize < sizeof(currentNumber) - 1) 
        {
            currentNumber[currentSize++] = ch;
        }
    }

    for (int i = 0; i < size; i++) {
        rooms->data[i].ptrArrayColliders = (roomDataSize[i] > 0) ? new int2[roomDataSize[i]] : nullptr;
        rooms->data[i].ptrArrayCrouchAreas = (roomDataSize2[i] > 0) ? new int2[roomDataSize2[i]] : nullptr;
        rooms->data[i].ptrArrayTeleportMarkers = (roomDataSize3[i] > 0) ? new int2[roomDataSize3[i]] : nullptr;
        rooms->data[i].ptrArrayVisibleCrouchAreas = (roomDataSize4[i] > 0) ? new int2[roomDataSize4[i]] : nullptr;

        rooms->data[i].colliderCount = roomDataSize[i];
        rooms->data[i].crouchAreaCount = roomDataSize2[i];
        rooms->data[i].teleportMakerCount = roomDataSize3[i];
        rooms->data[i].visibleCrouchAreaCount = roomDataSize4[i];

        // Reset counts for use in second pass
        roomDataSize[i] = 0;
        roomDataSize2[i] = 0;
        roomDataSize3[i] = 0;
        roomDataSize4[i] = 0;
    }

    mapSize = 0;
    currentSize = 0;

    for (size_t i = 0; i <= dataLength; i++) 
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n' || ch == '\0') 
        {
            if (currentSize > 0) 
            {
                currentNumber[currentSize] = '\0'; 
                int value = CharToInt(currentNumber); 

                int2 pos = { (mapSize % m_pitch) % ROOM_SIZE, (mapSize / m_pitch) % ROOM_SIZE };
                int2 mapPos = { (mapSize % m_pitch) / ROOM_SIZE, (mapSize / m_pitch) / ROOM_SIZE };
                int mapindex = mapPos.x + mapPos.y * m_mapRoomSize.x;

                if (mapindex >= 0 && mapindex < size) 
                {
                    switch (value) 
                    {
                        case markerValues::MARKER_COLLIDER:
                        {
                            rooms->data[mapindex].ptrArrayColliders[roomDataSize[mapindex]++] = pos;
                            break;

                        }
                        case markerValues::MARKER_CROUCH_AREA:
                        {
                            rooms->data[mapindex].ptrArrayCrouchAreas[roomDataSize2[mapindex]++] = pos;
                            break;
                        }
                        case markerValues::MARKER_TELEPORT_MARKER:
                        {
                            rooms->data[mapindex].ptrArrayTeleportMarkers[roomDataSize3[mapindex]++] = pos;
                            break;
                        }
                        case markerValues::MARKER_VISIBLE_CROUCH_AREA:
                        {
                            rooms->data[mapindex].ptrArrayVisibleCrouchAreas[roomDataSize4[mapindex]++] = pos;
                            break;
                        }
                        default:
                        break;
                    }
                }
                mapSize++;
                currentSize = 0;  // Reset for the next number
            }
        }
        else if (currentSize < sizeof(currentNumber) - 1) 
        {
            currentNumber[currentSize++] = ch;
        }
    }

    // Clean up heap meem
    delete[] roomDataSize;
    delete[] roomDataSize2;
    delete[] roomDataSize3;
    delete[] roomDataSize4;
    delete[] data;

    // Generate collision masks based on allocated marker positions
    for (int i = 0; i < rooms->size; i++) 
    {
        RoomStruct& room = rooms->data[i];
        Surface* mask = new Surface(ROOM_SIZE_IN_PIXELS, ROOM_SIZE_IN_PIXELS);
        mask->Clear(TRANSPARENCY_MASK);

        for (int j = 0; j < room.colliderCount; j++) 
        {
            int2 pos = room.ptrArrayColliders[j];
            pos *= TILE_SIZE;
            mask->Bar(pos.x, pos.y, pos.x + TILE_SIZE, pos.y + TILE_SIZE, 0x00ff00);
        }
        for (int j = 0; j < room.crouchAreaCount; j++) 
        {
            int2 pos = room.ptrArrayCrouchAreas[j];
            pos *= TILE_SIZE;
            mask->Bar(pos.x, pos.y, pos.x + TILE_SIZE, pos.y + TILE_SIZE, 0x00ff00);
        }

        room.collisionMask = mask;
    }
}


char* MapManager::GetCstringFromFile(const char* p_file) const
{
    FILE* file = fopen(p_file, "r");
    if (!file)
    {
        printf("Failed to open file: %s\n", p_file);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory for the file content (+1 for null terminator)
    char* buffer = new char[fileSize + 1];
    if (!buffer)
    {
        printf("Memory allocation failed\n");
        fclose(file);
        return nullptr;
    }

    // Read the file into the buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';  // Null terminate the buffer

    // Close the file
    fclose(file);

    return buffer;
}

DymArr<Enemy*>* MapManager::GetEnemiesFromCsv(char* p_csvFile, SpriteSheet* p_spritesheet, const int2* p_camPos)
{
    int enemyCount = 0;
    char* data = GetCstringFromFile(p_csvFile); // Use your file reading function
    size_t dataLength = strlen(data);
    int currentSize = 0;
    char* currentNumber = new char[5];

    /*
     index is used of reading how many emenies there are and which intruction they have
     index is also used to mark when the reading the values start and every index after index
     is getting the spawn pos of the enmies
    */
    int index = 0;

    //count how many enemies there are
    for (;index < dataLength; index++)
    {
        char ch = data[index];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);
                currentSize = 0; // Reset current size

                if (value == 21) break; // end of enemies marker
                if (value >= 0 && value <= 10) enemyCount++;
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    int currentEnemy = 0;
    int mapIndex = 0;

    // ptrArrayEnemies is a double ptr bcz its a ptr that points to a dynamic array
    Enemy** ptrArrayEnemies = new Enemy*[enemyCount];
    for (size_t i = 0; i < enemyCount; i++){ptrArrayEnemies[i] = nullptr;}

    // enemyInstructionCount is used to count total amount of instruction per enemy
    // enemyInstructionIndex is used to go trough them one by one
    int* enemyInstructionCount = new int[enemyCount];
    int* enemyInstructionIndex = new int[enemyCount];
    memset(enemyInstructionCount, 0, enemyCount * sizeof(int));
    memset(enemyInstructionIndex, 0, enemyCount * sizeof(int));

    // get the sizes of the 
    for (size_t i = 0; i < index; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);
                currentSize = 0; // Reset current size

                if (value == 21) break; // end of enemies marker
                if (value > 0 && value <= 10) currentEnemy++;
                else if (value >= 11 && value <= 18) enemyInstructionCount[currentEnemy]++;
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    Enemy::InstructionsStruct** ptrArrayInstructions = new Enemy::InstructionsStruct*[enemyCount];
    for (size_t i = 0; i < enemyCount; i++)
    {
        Enemy::InstructionsStruct* newInstruction = new Enemy::InstructionsStruct{new Enemy::Instruction[enemyInstructionCount[i]],  enemyInstructionCount[i]};
        ptrArrayInstructions[i] = newInstruction;
    }

    // reset vals for next itr
    currentEnemy = 0;
    currentSize = 0;
    currentNumber[currentSize] = '\0';

    for (size_t i = 0; i < dataLength; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);
                currentSize = 0; // Reset current size

                if (value == 21) break; // end of enemies marker
                if (value >= 0 && value < 10) currentEnemy = value;
                else if (value >= 11 && value <= 18)
                {
                    Enemy::InstructionsStruct* instructionstruct = ptrArrayInstructions[currentEnemy];

                    switch (value)
                    {
                        case markerValues::WALK_UP_ENEMY_INS: // walk up
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::walkUp };
                            enemyInstructionIndex[currentEnemy]++;

                            break;
                        }
                        case markerValues::WALK_RIGHT_ENEMY_INS: // walk right
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::walkRight };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WALK_DOWN_ENEMY_INS: // walk down
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::walkDown };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WALK_LEFT_ENEMY_INS: // walk left
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::walkLeft };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WAIT_UP_ENEMY_INS: // wait up
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::waitUp };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WAIT_RIGHT_ENEMY_INS: // wait right
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::waitRight };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WAIT_DOWN_ENEMY_INS: // wait down
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::waitDown };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                        case markerValues::WAIT_LEFT_ENEMY_INS: // wait left
                        {
                            instructionstruct->instructions[enemyInstructionIndex[currentEnemy]] = Enemy::Instruction{ {0,0},  Enemy::EnemyInstructions::waitLeft };
                            enemyInstructionIndex[currentEnemy]++;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    currentEnemy = 0;
    for (size_t i = 0; i < dataLength; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);
                currentSize = 0; // Reset current size
                if (value >= 0 && value < 10 && i > index)
                {
                    currentEnemy = value;
                    int2 pos = { mapIndex % m_pitch, mapIndex / m_pitch };
                    ptrArrayEnemies[currentEnemy] = new Enemy(p_spritesheet, p_camPos, ptrArrayInstructions[currentEnemy], pos, m_mapRoomSize);
                }
                mapIndex++;
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    // delte heap mem
    delete[] enemyInstructionCount;
    delete[] enemyInstructionIndex;
    return new DymArr<Enemy*>{ ptrArrayEnemies, enemyCount };
}

void MapManager::GetMapFromCsv(char* p_csvFile)
{
    int dataCount = 0;

    char* data = GetCstringFromFile(p_csvFile);

    // Count data elements (numbers) in the file
    for (int i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == ',' || data[i] == '\n')
        {
            dataCount++;
        }
    }
    dataCount++; // Add one for the last number

    int* dataArray = new int[dataCount];
    int index = 0;
    int pitch = GetLineCount(data);

    char currentNumber[6] = { 0 }; // Increased size to handle -1000 properly
    int currentSize = 0;

    // Parse the file data into an array of integers
    for (int i = 0; data[i] != '\0'; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Properly null terminate
                dataArray[index] = CharToInt(currentNumber);

                if (dataArray[index] < -99999 || dataArray[index] > 99999)
                {
                    printf("Warning: Value out of expected range at index %d: %d\n", index, dataArray[index]);
                }

                index++;
                currentSize = 0; // Reset current length
            }
        }
        else
        {
            // Add the character to currentNumber if within bounds
            if (currentSize < 5) // Allow space for null terminator
            {
                currentNumber[currentSize++] = ch;
            }
            else
            {
                printf("Warning: currentNumber buffer overflow detected!\n");
                currentSize = 0; // Reset on overflow
            }
        }
    }

    // Handle the last number if it wasn't followed by a newline
    if (currentSize > 0)
    {
        currentNumber[currentSize] = '\0'; // Properly null terminate
        dataArray[index++] = CharToInt(currentNumber);
    }

    delete[] data;

    m_ptrMapArray = dataArray;
    m_pitch = dataCount / pitch;
    int mapHeight = dataCount / m_pitch;
    m_roomSize = { m_pitch / m_mapRoomSize.x, mapHeight / m_mapRoomSize.y };


}

void MapManager::GetMapDataFromCsv(char* p_csvFile)
{
    int navNodesCount = 0;
    int markerCount = 0;
    int mapSize = 0;

    char* data = GetCstringFromFile(p_csvFile); // Use your file reading function
    size_t dataLength = strlen(data);
    int currentSize = 0;
    char* currentNumber = new char[5];
    // Count data elements (numbers) in the file
    for (int i = 0; i < dataLength; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);

                if (value >= 0 && value < 5)
                {
                    if (value == markerValues::MARKER_NODE)
                    {
                        navNodesCount++;
                    }
                    markerCount++;
                }
                else if (value == 5)
                {
                    int2 mapPos = { (mapSize % m_pitch) / ROOM_SIZE, (mapSize / m_pitch) / ROOM_SIZE };
                    //int roomIndex = static_cast<int>(mapPos.x * m_mapRoomSize.x + mapPos.y);
                    //roomDataSize[static_cast<int>(roomIndex)].x++;

                }
                else if (value == 6)
                {
                    int2 pos = { (mapSize % m_pitch) / ROOM_SIZE, (mapSize / m_pitch) / ROOM_SIZE };
                    //roomDataSize[static_cast<int>(pos.x * m_mapRoomSize.x + pos.y)].y++;
                }

                mapSize++;
                currentSize = 0; // Reset current size
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    // Handle the last number if any
    if (currentSize > 0)
    {
        currentNumber[currentSize] = '\0'; // Null terminate the last number
        int value = CharToInt(currentNumber);
        if (value >= markerValues::MARKER_NODE && value < markerValues::MARKER_COLLIDER)
        {
            if (value == markerValues::MARKER_NODE)
            {
                navNodesCount++;
            }
            markerCount++;
        }
    }


    // Allocate memory for navMesh and markers
    NavNode* navMesh = new NavNode[navNodesCount];
    for (int i = 0; i < navNodesCount; i++) navMesh[i] = { nullptr, nullptr, nullptr, nullptr, {0,0} };

    Marker** markers = new Marker * [markerCount];
    for (int i = 0; i < markerCount; i++) markers[i] = nullptr;

    TempNavNode* tempNavNodes = new TempNavNode[navNodesCount];
    for (int i = 0; i < navNodesCount; i++) tempNavNodes[i] = { -1, -1, -1, -1, {0,0} };

    int navmeshIndex = 0;
    int markerIndex = 0;
    int pitch = mapSize / GetLineCount(data);
    int mapIndex = 0;

    // Parse the file data into navMesh and markers
    currentSize = 0; // Reset for the next parsing
    for (int i = 0; i < dataLength; i++)
    {
        char ch = data[i];

        if (ch == ',' || ch == '\n')
        {
            if (currentSize > 0)
            {
                currentNumber[currentSize] = '\0'; // Null terminate the current number
                int value = CharToInt(currentNumber);

                if (value > -1 && value < 5)
                {
                    int2 pos = { mapIndex % pitch, mapIndex / pitch };
                    markers[markerIndex] = new Marker{ pos, value };

                    if (value == markerValues::MARKER_NODE)
                    {
                        navMesh[navmeshIndex] = { nullptr, nullptr, nullptr, nullptr, pos };
                        tempNavNodes[navmeshIndex] = { -1, -1, -1, -1, pos };
                        navmeshIndex++;
                    }

                    markerIndex++;
                }

                currentSize = 0; // Reset for the next number
                mapIndex++;
            }
        }
        else
        {
            // Add the character to currentNumber
            currentNumber[currentSize++] = ch;
        }
    }

    // Final handling for the last number if any
    if (currentSize > 0)
    {
        currentNumber[currentSize] = '\0'; // Null terminate the last number
        int value = CharToInt(currentNumber);
        if (value > -1 && value < markerValues::MARKER_COLLIDER)
        {
            int2 pos = { mapIndex % pitch, mapIndex / pitch };
            markers[markerIndex] = new Marker{ pos, value };

            if (value == 0)
            {
                navMesh[navmeshIndex] = { nullptr, nullptr, nullptr, nullptr, pos };
                tempNavNodes[navmeshIndex] = { -1, -1, -1, -1, pos };
                navmeshIndex++;
            }
            markerIndex++;
        }
    }

    // Connect markers to navMesh nodes and assign values
    for (size_t i = 0; i < navNodesCount; i++)
    {
        TempNavNode& currentNode = tempNavNodes[i];

        // Correct coordinate assignments
        int2 t = { currentNode.pos.x, currentNode.pos.y - 1 };
        int2 r = { currentNode.pos.x + 1, currentNode.pos.y };
        int2 b = { currentNode.pos.x, currentNode.pos.y + 1 };
        int2 l = { currentNode.pos.x - 1, currentNode.pos.y };

        // Compare each marker to the current node's surrounding positions
        for (size_t j = 0; j < markerCount; j++)
        {
            if (markers[j] == nullptr) continue;

            int2 pos = markers[j]->pos;

            if (pos.x == t.x && pos.y == t.y)
            {
                currentNode.markerValT = markers[j]->val;
            }
            else if (pos.x == r.x && pos.y == r.y)
            {
                currentNode.markerValR = markers[j]->val;
            }
            else if (pos.x == b.x && pos.y == b.y)
            {
                currentNode.markerValB = markers[j]->val;
            }
            else if (pos.x == l.x && pos.y == l.y)
            {
                currentNode.markerValL = markers[j]->val;
            }
        }
    }

    // Connect the nodes
    for (size_t i = 0; i < navNodesCount; i++)
    {
        NavNode& currentNode = navMesh[i];

        if (currentNode.t != nullptr && currentNode.r != nullptr &&
            currentNode.b != nullptr && currentNode.l != nullptr)
            continue; // Skip if all connections are already made

        int t = tempNavNodes[i].markerValT;
        int r = tempNavNodes[i].markerValR;
        int b = tempNavNodes[i].markerValB;
        int l = tempNavNodes[i].markerValL;


        for (size_t j = 0; j < navNodesCount; j++)
        {
            if (i == j) continue; // Skip self

            NavNode& checkedNode = navMesh[j];


            // Check the vertical (y-axis) connection
            if (currentNode.pos.x == checkedNode.pos.x)
            {
                if (currentNode.pos.y < checkedNode.pos.y) // If current is above
                {

                    if (currentNode.b == nullptr && b == tempNavNodes[j].markerValT && b != -1)
                    {
                        currentNode.b = &checkedNode;
                        checkedNode.t = &currentNode;
                        continue;
                    }
                }
                else // Current is below
                {
                    if (currentNode.t == nullptr && t == tempNavNodes[j].markerValB && t != -1)
                    {

                        currentNode.t = &checkedNode;
                        checkedNode.b = &currentNode;
                        continue;
                    }

                   
                }
            }
            else if (currentNode.pos.y == checkedNode.pos.y)
            {
                if (currentNode.pos.x < checkedNode.pos.x) // If current is to the left
                {
                    if (currentNode.r == nullptr && r == tempNavNodes[j].markerValL && r != -1)
                    {
                        currentNode.r = &checkedNode;
                        checkedNode.l = &currentNode;
                        continue;
                    }
                }
                else
                {
                    if (currentNode.l == nullptr && l == tempNavNodes[j].markerValR && l != -1)
                    {
                        currentNode.l = &checkedNode;
                        checkedNode.r = &currentNode;
                        continue;
                    }
                }
            }
        }
    }

    // Cleanup
    delete[] data;
    delete[] currentNumber;

    // Cleanup markers
    for (size_t i = 0; i < markerCount; i++)
    {
        delete markers[i];
    }
    delete[] markers;

    m_navNodes = { navMesh, navNodesCount };
}

int MapManager::CharToInt(const char* p_char) const
{
    int number = 0;
    bool isNegative = false;
    int i = 0;

    if (p_char[0] == '-')
    {
        isNegative = true;
        i++;
    }

    for (; p_char[i] != '\0'; i++)
    {
        number = number * 10 + (p_char[i] - '0');
    }

    if (number > 99999 || number < -99999)
    {
        printf("error:number shouldnt exceed 5 digits, in MapManager::CharToInt\n");
    }

    return isNegative ? -number : number;
}

