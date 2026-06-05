// Fill out your copyright notice in the Description page of Project Settings.



#include "Math/MathEngine.h"


/*// 被裁减多边形上面的点，需要记录该点与裁剪窗口的关系
USTRUCT()
struct FPrPoint
{
	GENERATED_BODY()
	FVector2D Pt;
	int32 PtFlag; // 0:普通点  1:入点  2:出点

	bool operator == (const FPrPoint &PPt) const
	{
		if (FMath::Abs(this->Pt.X - PPt.Pt.X) < SMALL_NUMBER && FMath::Abs(this->Pt.Y - PPt.Pt.Y) < SMALL_NUMBER)
		{
			return true;
		}
		return false;
	}
};

USTRUCT()
struct FPrRect
{
	GENERATED_BODY()
	double left;
	double right;
	double top;
	double bottom;
};


//交点
USTRUCT()
struct FIntersecPt
{
	GENERATED_BODY()
	int32 Index;          //插入的序号
	float Distance;       //交点到交点前一个点的距离
	FPrPoint IntersecPt;  //待插入的交点
	FPrPoint PrePt;       //交点前面一个点
};*/


//矩形的边
/*UENUM()
enum my_enum {  };*/
UENUM()
enum  RectEdge
{
	LEFT = 0,
	TOP,
	RIGHT,
	BOTTOM,
};








/*
// Weiler-Atherton算法
void ATerrainActor::CropPolygon(TArray<FPrPoint>& CropRect, TArray<FPrPoint>& CroppedPolygon)
{
	int32 RectEdgeNum = CropRect.Num();
	int32 PolyEdgeNum = CroppedPolygon.Num();

	TArray<FIntersecPt> IntersecPoly; // Polygon待插入的点
	TArray<FIntersecPt> IntersecRect; // Rect待插入的点
 
	// 找出交点及点与多边形的关系
	for (int32 i = 0; i < RectEdgeNum; i++)// 裁剪矩形
	{
		FPrPoint StartRectPt = CropRect[i % RectEdgeNum];
		FPrPoint EndRectPt = CropRect[(i+1) % RectEdgeNum];

		for (int32 j = 0; j < PolyEdgeNum; j++)// 被裁剪的多边形
		{
			FPrPoint StartPolyPt = CroppedPolygon[j % PolyEdgeNum];
			FPrPoint EndPolyPt   = CroppedPolygon[(j+1) % PolyEdgeNum];

			FPrPoint IterPt{FVector2D(-1, -1), 0};
			if (LLCrossPoint((RectEdge)i, StartRectPt, EndRectPt, StartPolyPt, EndPolyPt, IterPt))
			{
				FIntersecPt TempInter;
				TempInter.Index = i;
				TempInter.Distance = FVector2D::Distance(IterPt.Pt, StartRectPt.Pt);
				TempInter.IntersecPt = IterPt;
				TempInter.PrePt = StartRectPt;
				IntersecRect.Add(TempInter);

				TempInter.Index = j;
				TempInter.Distance = FVector2D::Distance(IterPt.Pt, StartPolyPt.Pt);
				TempInter.IntersecPt = IterPt;
				TempInter.PrePt = StartPolyPt;
				IntersecPoly.Add(TempInter);
			}
		}
	}

	// 根据交点前一个点和距离的大小来排序
	// 这里的参数为啥要加const？？？？？
	// 这种写法正确嘛？？？
	IntersecRect.Sort([](const FIntersecPt& A, const FIntersecPt& B){
		if (FMath::Abs(A.PrePt.Pt.X - B.PrePt.Pt.X) < SMALL_NUMBER)
		{
            return A.Distance > B.Distance;
        }
        else
        {
            return A.IntersecPt.Pt.X < B.IntersecPt.Pt.X;
        }
	});

	IntersecPoly.Sort([](const FIntersecPt& A, const FIntersecPt& B){
		if (FMath::Abs(A.PrePt.Pt.X - B.PrePt.Pt.X) < SMALL_NUMBER)
		{
            return A.Distance > B.Distance;
        }
        else
        {
            return A.IntersecPt.Pt.X < B.IntersecPt.Pt.X;
        }
	});

	// 插入交点到裁剪多边形和被裁剪多边形
	// 插入到裁剪多边形--矩形
	for (int32 i = 0; i < IntersecRect.Num(); i++)
	{
		bool Flag = false;
		int32 j = IntersecRect[i].Index;
		while (!Flag)
		{
			for (; j < CropRect.Num(); j++)
			{
				if (CropRect[j] == IntersecRect[i].PrePt)
				{
					Flag = true;
					CropRect.Insert(IntersecRect[i].IntersecPt, j + 1);
					break;
				}
			}
			// 没找到待插入点的前一个点，从队列的第一个位置开始找
			j = 0;
		}
	}

	// 插入到被裁剪多边形
	for (int32 i = 0; i < IntersecPoly.Num(); i++)
	{
		bool Flag = false;
		int32 j = IntersecRect[i].Index;
		while (!Flag)
		{
			for (; j < CroppedPolygon.Num(); j++)
			{
				if (CroppedPolygon[j] == IntersecPoly[i].PrePt)
				{
					Flag = true;
					CroppedPolygon.Insert(IntersecPoly[i].IntersecPt, j + 1);
					break;
				}
			}
			// 没找到待插入点的前一个点，从队列的第一个位置开始找
			j = 0;
		}
	}

	// 从被裁剪多边形中查找点
	int CropStatus = 0;
	TArray<FPrPoint> SubPolygon;
	PolygonArr.Empty();

	for (int32 i = 0; i < CroppedPolygon.Num(); i++)
	{
		switch(CropStatus)
		{
		case 0: // 先寻找到一个入点，找到之后把该点置为普通点
			{
				if (CroppedPolygon[i].PtFlag == 1)
				{
					CroppedPolygon[i].PtFlag = 0;
					SubPolygon.Add(CroppedPolygon[i]);
					CropStatus++;
				}
			}
			break;
		// 找到入点之后需要去寻找出点，在找到出点前把所有的普通点放入裁剪数组中
		// 找到出点之后需要转换到裁剪多边形（矩形窗口）上面去寻找下一个点
		case 1:
			{
				if (CroppedPolygon[i].PtFlag == 2)
				{
					CropStatus++;
				}
				CroppedPolygon[i].PtFlag = 0;
				SubPolygon.Add(CroppedPolygon[i]);

				// 当被裁剪多边形的最后一个点是出点,则表示查找裁剪后的多边形完成
				if (CropStatus == 2 && i == CroppedPolygon.Num() - 1)
				{
					PolygonArr.Add(SubPolygon);
					SubPolygon.Empty();
				}
				// 找到入点后寻找普通点找到最后一个点之后，需要将外部大循环重新开始
				if (i == CroppedPolygon.Num() - 1)
				{
					i = -1;
				}
			}
			break;
		case 2:
			{
				// 从裁剪多边形中查找点
				int RectStatus = 0;
				FPrPoint StartPt = SubPolygon[0];
				while (CropStatus != 0)
				{
					for (int32 j = 0; j < CropRect.Num(); j++)
					{
						if (CropStatus == 0)
						{
							break;
						}
						// 一个裁剪多边形查找完成
						if (RectStatus != 0 && StartPt == CropRect[j])
						{
							CropStatus = 0;
							CropRect[j].PtFlag = 0;
							PolygonArr.Add(SubPolygon);
							SubPolygon.Empty();
							break;
						}
						switch(RectStatus)
						{
						case 0: // 寻找裁剪多边形（矩形窗口）上的入点
							{
								// 找到裁剪多边形上面的起始点（即裁剪数组的最后一个点），然后转入到找入点的步骤
								if (CropRect[j] == SubPolygon.Last())
								{
									RectStatus++;
									CropRect[j].PtFlag = 0;
								}
							}
							break;
						case 1: // 再去寻找裁剪多边形上面的入点
							{
								// 找到入点了，说明要离开裁剪多边形（矩形窗口了）
								if (CropRect[j].PtFlag == 1)
								{
									RectStatus++;
									CropStatus = 0;
									CropRect[j].PtFlag = 0;
								}
								// 裁剪多边形（矩形窗口上的普通点）
								else
								{
									CropRect[j].PtFlag = 0;
									SubPolygon.Add(CropRect[j]);
								}
							}
							break;
						default:
							break;
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}


// 计算被裁剪多边形与裁剪窗口边的交点及位置关系
bool ATerrainActor::LLCrossPoint(RectEdge EdgeType, FPrPoint& StartRect, FPrPoint& EndRect, FPrPoint& StartPoly, FPrPoint& EndPoly, FPrPoint& InterPt)
{
	
	double A1, B1, C1;
	CalcLinePara(StartRect.Pt, EndRect.Pt, A1, B1, C1);
	double A2, B2, C2;
	CalcLinePara(StartPoly.Pt, EndPoly.Pt, A2, B2, C2);

	// 平行直线或共线
	if(FMath::Abs(A1*B2-A2*B1) < SMALL_NUMBER)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2000, FColor::Green, TEXT("没有交点"));
		return false;
	}

	//两条线段所在两条直线的交点
	InterPt.Pt.Y = (C1*A2-C2*A1)/(B2*A1-B1*A2);
	InterPt.Pt.X = (B1*C2-B2*C1)/(B2*A1-B1*A2);

	//GEngine->AddOnScreenDebugMessage(-1, 2000, FColor::Green, InterPt.Pt.ToString());
	//bool IsIntersec = false;
	//如果交点同时在两条线段上面，说明线段有交点
	bool IsIntersec = PtOnSegment(StartRect.Pt,EndRect.Pt,InterPt.Pt) && PtOnSegment(StartPoly.Pt,EndPoly.Pt,InterPt.Pt);

	if (IsIntersec)
	{
		//出入点的判断,这叫点怎么这么多
		switch(EdgeType)
		{
		case LEFT:
                if (StartPoly.Pt.X<InterPt.Pt.X && InterPt.Pt.X<EndPoly.Pt.X)//入点
                	{
                	InterPt.PtFlag = 1;
                	}
			else if (EndPoly.Pt.X<InterPt.Pt.X && InterPt.Pt.X<StartPoly.Pt.X)//出点
				{
				InterPt.PtFlag = 2;
				}
			break;
		case RIGHT:
            if (EndPoly.Pt.X<InterPt.Pt.X && InterPt.Pt.X<StartPoly.Pt.X)//入点
            	{
            	InterPt.PtFlag = 1;
            	}
			else if (StartPoly.Pt.X<InterPt.Pt.X && InterPt.Pt.X<EndPoly.Pt.X)//出点
				{
				InterPt.PtFlag = 2;
				}
			break;
		case TOP:
            if (EndPoly.Pt.Y<InterPt.Pt.Y && InterPt.Pt.Y<StartPoly.Pt.Y)//入点
            	{
            	InterPt.PtFlag = 1;
            	}
			else if (StartPoly.Pt.Y<InterPt.Pt.Y && InterPt.Pt.Y<EndPoly.Pt.Y)//出点
				{
				InterPt.PtFlag = 2;
				}
			break;
		case BOTTOM:
            if (StartPoly.Pt.Y<InterPt.Pt.Y && InterPt.Pt.Y<EndPoly.Pt.Y)//入点
            	{
            	InterPt.PtFlag = 1;
            	}
			else if (EndPoly.Pt.Y<InterPt.Pt.Y && InterPt.Pt.Y<StartPoly.Pt.Y)//出点
				{
				InterPt.PtFlag = 2;
				}
			break;
		}
	}
	return IsIntersec;
}



// 判断线段与矩形的相交方向，该矩形顺序必须为逆时针方向
// 若相交，IntersectionDir中存储该交点的前进方向，并返回true
bool ATerrainActor::SegIntersectionRect(TArray<FVector2D>& RectVertex, FVector2D& StartPt, FVector2D& EndPt, TArray<FIntPoint>& IntersectionDir)
{
	/*FVector StartPtA(StartPt.X, StartPt.Y, 0);
	FVector EndPtA(EndPt.X, EndPt.Y, 0);#1#
	FVector2D IntersectionPt;
	for (int32 i = 0; i < 4; i++)
	{
		FVector2D StartPtB(RectVertex[i%4].X,RectVertex[i%4].Y);
		FVector2D EndPtB(RectVertex[(i + 1)%4].X,RectVertex[(i + 1)%4].Y);
		// 系统自带的两线段计算交点函数经度太低了，草
		if(SegmentIntersection(StartPt, EndPt, StartPtB, EndPtB, IntersectionPt))
		{
			if (IntersectionPt.Equals(EndPtB, SMALL_NUMBER))
			{
				IntersectionDir.Add(FIntPoint(FMath::RoundToInt(FMath::Cos((float)(2*i + 1)/4*PI)),
					FMath::RoundToInt(FMath::Sin((float)(2*i + 1)/4*PI))));
			}
			else
			{
				IntersectionDir.Add(FIntPoint(FMath::RoundToInt(FMath::Cos((float)(2*i)/4*PI)),
					FMath::RoundToInt(FMath::Sin((float)(2*i)/4*PI))));
			}
		}
	}
	return IntersectionDir.Num() > 0;
}*/