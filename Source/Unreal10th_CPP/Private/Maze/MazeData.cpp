// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeData.h"
#include "Maze/CellData.h"

const FIntPoint FMazeData::Direction[DirectionCount] =
{
	FIntPoint(0,1), FIntPoint(1,0), FIntPoint(0,-1), FIntPoint(-1,0)
};

FMazeData::~FMazeData()
{
	ClearMaze();	// 안전장치
}

void FMazeData::MakeMaze(uint8 InWidth, uint8 InHeight, EMazeAlgorithm InAlgorithm, int32 InSeed)
{
	ClearMaze();					// 이전에 만들어진 데이터 삭제하기

	Width = FMath::Clamp(InWidth, MinimumSize, MaximumSize);	// 가로 세로 크기는 3~100으로 한정
	Height = FMath::Clamp(InHeight, MinimumSize, MaximumSize);

	if (InSeed == RandomSeed)
	{
		// 랜덤하게 시드값 설정
		RandomStream.GenerateNewSeed();
	}
	else
	{
		// 정해진 값으로 시드값 설정
		RandomStream.Initialize(InSeed);
	}

	Cells.SetNum(Width * Height);	// 배열 초기화(SetNum으로 실제 배열 요소도 생성)

	switch (InAlgorithm)
	{
	case EMazeAlgorithm::Wilson:
		WilsonAlgorithmExecute();
		break;
	case EMazeAlgorithm::RecursiveBacktracking:
		RecursiveBacktrackingAlgorithmExecute();
		break;
	case EMazeAlgorithm::Eller:
		EllerAlgorithmExecute();
		break;
	default:
		WilsonAlgorithmExecute();	// 윌슨이 디폴트
		break;
	}
}

void FMazeData::ClearMaze()
{
	Width = 0;
	Height = 0;
	Cells.Empty();
}

void FMazeData::WilsonAlgorithmExecute()
{
	// 윌슨 알고리즘 전용 연속 메모리 임시 배열 (Scratchpad)
	TArray<FWilsonCellData> WillsonCells;
	WillsonCells.SetNum(Width * Height);

	TArray<FWilsonCellData*> NotInMazeCells;
	NotInMazeCells.Reserve(Width * Height);

	for (uint8 y = 0; y < Height; y++)
	{
		for (uint8 x = 0; x < Width; x++)
		{
			uint16 Index = LocationToIndex(x, y);
			FWilsonCellData& Cell = WillsonCells[Index];
			Cell.X = x;
			Cell.Y = y;
			Cell.Path = EDirectionType::None;
			Cell.bInMaze = false;
			Cell.NextCell = nullptr;
			NotInMazeCells.Add(&Cell);
		}
	}
	ShuffleArray(NotInMazeCells);

	// 윌슨 알고리즘 시작
	FWilsonCellData* InitCell = NotInMazeCells.Pop();	// 미로가 아닌 셀 중 하나를 초기셀로 지정하고 셀 목록에서 제거
	InitCell->bInMaze = true;

	while (NotInMazeCells.Num() > 0)	// 미로에 포함되지 않은 셀이 남아있으면 계속 반복
	{	
		FWilsonCellData* StartCell = NotInMazeCells.Pop();	// 미로에 포함되지 않은 셀 중 하나를 랜덤으로 선택
		if (StartCell->bInMaze)
		{
			continue;	// 이미 미로에 포함된 셀은 스킵(NotInMazeCells에서 제거하는 역할)
		}

		FWilsonCellData* CurrentCell = StartCell;
		do
		{
			// 랜덤 워크 진행
			FIntPoint NeighborLoc;
			do
			{
				int32 DirIdx = RandomStream.RandRange(0, DirectionCount - 1);
				NeighborLoc = CurrentCell->GetLocation() + Direction[DirIdx];
			} while (!IsValidLocation(NeighborLoc.X, NeighborLoc.Y));

			FWilsonCellData* NeighborCell = &WillsonCells[LocationToIndex(NeighborLoc.X, NeighborLoc.Y)];
			CurrentCell->NextCell = NeighborCell;	// 다음 셀로 저장하고
			CurrentCell = NeighborCell;				// 이웃 셀 기준으로 계속 진행
		} while (!CurrentCell->bInMaze);

		// 경로에 따라 미로에 포함시키기
		FWilsonCellData* Path = StartCell;
		while (Path != CurrentCell)	// 미로에 포함되어 있는 셀에 도달할 때까지 반복
		{
			Path->bInMaze = true;					// 미로에 포함시키고
			ConnectCells(Path, Path->NextCell);		// 경로를 뚫고
			Path = Path->NextCell;					// 다음 셀 처리
		}
	}

	// 생성 결과를 FMazeData의 연속 메모리 배열 Cells로 전달
	CopyToCells(WillsonCells);
}

/*
 * =========================================================================================
 * 리커시브 백트래킹(Recursive Backtracking / DFS 기반) 미로 생성 알고리즘
 * =========================================================================================
 * [개요]
 * - 깊이 우선 탐색(DFS, Depth-First Search) 및 스택(Stack)을 이용한 미로 생성 알고리즘입니다.
 * - 임의의 시작 셀에서 출발하여 방문하지 않은 이웃 셀로 무작위 이동하며 경로를 허물어 나갑니다.
 * - 더 이상 이동할 수 있는 미방문 이웃 셀이 없을 경우, 이전 셀로 백트래킹(Stack Pop)하며 새로운 경로를 재탐색합니다.
 * - 특성: 긴 통로와 적은 분기점을 가지는 굽이치는 형태의 미로가 생성됩니다.
 * =========================================================================================
 */
void FMazeData::RecursiveBacktrackingAlgorithmExecute()
{
	// 1. 리커시브 백트래킹 전용 임시 작업 배열(Scratchpad) 생성 및 초기화
	TArray<FRecursiveBacktrackingCellData> BacktrackingCells;
	BacktrackingCells.SetNum(Width * Height);

	// 각 셀의 좌표 및 초기화 (길 없음, 방문 안 함)
	for (uint8 y = 0; y < Height; y++)
	{
		for (uint8 x = 0; x < Width; x++)
		{
			uint16 Index = LocationToIndex(x, y);
			FRecursiveBacktrackingCellData& Cell = BacktrackingCells[Index];
			Cell.X = x;
			Cell.Y = y;
			Cell.Path = EDirectionType::None;
			Cell.bVisited = false; // 미방문 상태로 설정
		}
	}

	// 2. 백트래킹 탐색을 위한 스택 생성 (최대 셀 개수만큼 공간 예약)
	TArray<FRecursiveBacktrackingCellData*> Stack;
	Stack.Reserve(Width * Height);

	// 3. 미로 생성을 시작할 첫 번째 셀을 랜덤하게 선택
	uint8 StartX = static_cast<uint8>(RandomStream.RandRange(0, Width - 1));
	uint8 StartY = static_cast<uint8>(RandomStream.RandRange(0, Height - 1));

	// 시작 셀을 방문 처리하고 스택에 Push
	FRecursiveBacktrackingCellData* StartCell = &BacktrackingCells[LocationToIndex(StartX, StartY)];
	StartCell->bVisited = true;
	Stack.Push(StartCell);

	// 4. 스택에 처리할 셀이 남아있는 동안 백트래킹 탐색 반복
	while (Stack.Num() > 0)
	{
		// 스택의 최상단(Top) 셀을 현재 기준으로 설정
		FRecursiveBacktrackingCellData* CurrentCell = Stack.Top();

		// 현재 셀의 4방향 이웃 중 아직 방문하지 않은 이웃 셀들을 수집할 배열
		TArray<FRecursiveBacktrackingCellData*> UnvisitedNeighbors;
		UnvisitedNeighbors.Reserve(DirectionCount);

		// 상하좌우 4방향 이웃 탐색
		for (int32 i = 0; i < DirectionCount; i++)
		{
			FIntPoint NeighborLoc = CurrentCell->GetLocation() + Direction[i];
			// 이웃 좌표가 미로 범위를 벗어나지 않는지 검사
			if (IsValidLocation(NeighborLoc.X, NeighborLoc.Y))
			{
				FRecursiveBacktrackingCellData* NeighborCell = &BacktrackingCells[LocationToIndex(NeighborLoc.X, NeighborLoc.Y)];
				// 아직 방문하지 않은 셀인 경우에만 추가
				if (!NeighborCell->bVisited)
				{
					UnvisitedNeighbors.Add(NeighborCell);
				}
			}
		}

		// 5. 방문하지 않은 이웃 셀이 존재하는 경우
		if (UnvisitedNeighbors.Num() > 0)
		{
			// 방문하지 않은 이웃 중 하나를 랜덤으로 선택
			int32 RandIdx = RandomStream.RandRange(0, UnvisitedNeighbors.Num() - 1);
			FRecursiveBacktrackingCellData* NextCell = UnvisitedNeighbors[RandIdx];

			// 현재 셀과 선택한 이웃 셀 사이의 벽을 허물어 경로 연결
			ConnectCells(CurrentCell, NextCell);

			// 선택한 이웃 셀을 방문 처리하고 스택에 Push하여 계속 깊이 탐색 진행
			NextCell->bVisited = true;
			Stack.Push(NextCell);
		}
		// 6. 방문하지 않은 이웃이 더 이상 없는 경우 (막다른 길)
		else
		{
			// 스택에서 현재 셀을 제거하여 이전 셀로 돌아감 (Backtrack)
			Stack.Pop();
		}
	}

	// 7. 작업용 스크래치패드의 최종 생성 결과(Path 등)를 FMazeData의 연속 메모리 Cells 배열로 복사
	CopyToCells(BacktrackingCells);
}

/*
 * =========================================================================================
 * 엘러(Eller's Algorithm) 미로 생성 알고리즘
 * =========================================================================================
 * [개요]
 * - 행(Row) 단위로 한 줄씩 미로를 생성해 나가는 알고리즘입니다.
 * - 각 셀에 집합 ID(Set ID)를 부여하고, 통로를 뚫을 때마다 집합을 병합(Merge)합니다.
 * - 주요 3단계 (각 행마다 수행):
 *   1) 집합 할당: 현재 행의 집합이 없는 셀들에 서로 다른 고유한 Set ID를 부여합니다.
 *   2) 가로 연결: 인접한 셀들이 서로 다른 집합일 때 무작위로 가로 통로를 뚫고 집합을 통합합니다.
 *                 (단, 마지막 행에서는 서로 다른 집합인 인접 셀을 무조건 모두 연결)
 *   3) 세로 연결: 각 집합에서 최소 1개 이상의 셀을 선택하여 아래 행(South)으로 세로 통로를 뚫습니다.
 * - 특성: 메모리 효율성이 뛰어나고 행 단위 스트리밍 처리가 가능한 균형 잡힌 미로가 생성됩니다.
 * =========================================================================================
 */
void FMazeData::EllerAlgorithmExecute()
{
	// 1. 엘러 알고리즘 전용 임시 작업 배열(Scratchpad) 생성 및 초기화
	TArray<FEllerCellData> EllerCells;
	EllerCells.SetNum(Width * Height);

	// 각 셀의 좌표 및 기본값(길 없음, 집합 ID 0) 설정
	for (uint8 y = 0; y < Height; y++)
	{
		for (uint8 x = 0; x < Width; x++)
		{
			uint16 Index = LocationToIndex(x, y);
			FEllerCellData& Cell = EllerCells[Index];
			Cell.X = x;
			Cell.Y = y;
			Cell.Path = EDirectionType::None;
			Cell.SetID = 0; // 아직 집합에 속하지 않음을 나타냄
		}
	}

	// 새로운 집합을 발급할 때 사용할 순차적 ID 카운터
	int32 NextSetID = 1;

	// 2. 첫 번째 행부터 마지막 행까지 한 줄(Row)씩 순차적으로 처리
	for (uint8 y = 0; y < Height; y++)
	{
		// [단계 1] 현재 행의 셀 중 아직 집합 ID가 없는 셀들에 고유한 집합 ID 부여
		for (uint8 x = 0; x < Width; x++)
		{
			FEllerCellData& Cell = EllerCells[LocationToIndex(x, y)];
			if (Cell.SetID == 0)
			{
				Cell.SetID = NextSetID++;
			}
		}

		// [단계 2] 가로(동쪽) 방향 연결 처리
		for (uint8 x = 0; x < Width - 1; x++)
		{
			FEllerCellData& CurrentCell = EllerCells[LocationToIndex(x, y)];
			FEllerCellData& NextCell = EllerCells[LocationToIndex(x + 1, y)];

			// 두 인접 셀이 서로 다른 집합에 속해 있을 때만 연결 후보가 됨 (순환/루프 방지)
			if (CurrentCell.SetID != NextCell.SetID)
			{
				// 마지막 행인 경우: 무조건 연결 (미로 분리 방지)
				// 일반 행인 경우: 50% 확률로 무작위 연결 결정
				bool bConnect = (y == Height - 1) || (RandomStream.RandRange(0, 1) == 0);
				if (bConnect)
				{
					// 동-서 방향 벽을 허물어 길을 형성
					ConnectCells(&CurrentCell, &NextCell);

					int32 TargetSetID = NextCell.SetID;
					int32 SourceSetID = CurrentCell.SetID;

					// 다음 셀의 집합(TargetSetID)을 현재 셀의 집합(SourceSetID)으로 전체 병합
					for (uint8 k = 0; k < Width; k++)
					{
						FEllerCellData& RowCell = EllerCells[LocationToIndex(k, y)];
						if (RowCell.SetID == TargetSetID)
						{
							RowCell.SetID = SourceSetID;
						}
					}
				}
			}
		}

		// [단계 3] 세로(남쪽) 방향 연결 처리 (마지막 행은 아래 행이 없으므로 제외)
		if (y < Height - 1)
		{
			// 현재 행에 존재하는 각 집합(SetID)별로 포함된 셀의 X좌표 목록을 분류 수집
			TMap<int32, TArray<uint8>> SetToXMap;
			for (uint8 x = 0; x < Width; x++)
			{
				FEllerCellData& Cell = EllerCells[LocationToIndex(x, y)];
				SetToXMap.FindOrAdd(Cell.SetID).Add(x);
			}

			// 수집된 각 집합별로 아래 행과 세로 연결 진행
			for (auto& Pair : SetToXMap)
			{
				TArray<uint8>& XList = Pair.Value;

				// 해당 집합의 X좌표 목록을 무작위로 섞음 (무작위 세로 통로 선택)
				for (int32 i = XList.Num() - 1; i > 0; i--)
				{
					int32 SwapIdx = RandomStream.RandRange(0, i);
					XList.Swap(i, SwapIdx);
				}

				// 고립 지역 방지를 위해 각 집합마다 최소 1개 이상 ~ 최대 전체 개수만큼 세로 연결 개수 결정
				int32 VerticalCount = RandomStream.RandRange(1, XList.Num());
				for (int32 i = 0; i < VerticalCount; i++)
				{
					uint8 x = XList[i];
					FEllerCellData& CurrentCell = EllerCells[LocationToIndex(x, y)];
					FEllerCellData& DownCell = EllerCells[LocationToIndex(x, y + 1)];

					// 남-북 방향 벽을 허물어 세로 경로 형성
					ConnectCells(&CurrentCell, &DownCell);
					// 아래 행의 셀에 현재 집합 ID를 전파(상속)
					DownCell.SetID = CurrentCell.SetID;
				}
			}
		}
	}

	// 3. 작업용 스크래치패드의 최종 생성 결과(Path 등)를 FMazeData의 연속 메모리 Cells 배열로 복사
	CopyToCells(EllerCells);
}

FCellData* FMazeData::GetCell(uint8 InX, uint8 InY)
{
	if (!IsValidLocation(InX, InY)) return nullptr;

	return &(Cells[LocationToIndex(InX, InY)]);
}

void FMazeData::ConnectCells(FCellData* InFrom, FCellData* InTo)
{
	// From과 To가 모두 있어야 하고, 서로 다른 셀이어야 한다.
	if (!InFrom || !InTo || (InFrom == InTo)) return;

	if (InFrom->X < InTo->X)
	{
		// From의 동쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::East);
		InTo->AddPath(EDirectionType::West);
	}
	else if (InFrom->X > InTo->X)
	{
		// From의 서쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::West);
		InTo->AddPath(EDirectionType::East);
	}
	else if (InFrom->Y < InTo->Y)
	{
		// From의 남쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::South);
		InTo->AddPath(EDirectionType::North);
	}
	else //if (InFrom->Y > InTo->Y)
	{
		// From의 북쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::North);
		InTo->AddPath(EDirectionType::South);
	}
}

FCellData* FMazeData::GetRandomNeighborCell(const FCellData& InCell)
{
	FIntPoint NeighborLoc;

	do
	{
		int32 Index = RandomStream.RandRange(0, DirectionCount - 1); // 0,1,2,3 중 하나가 랜덤으로 결정
		NeighborLoc = InCell.GetLocation() + Direction[Index];
	} while (!IsValidLocation(NeighborLoc.X, NeighborLoc.Y));	// 미로 밖을 선택하는 일을 방지

	return GetCell(static_cast<uint8>(NeighborLoc.X), static_cast<uint8>(NeighborLoc.Y));
}
