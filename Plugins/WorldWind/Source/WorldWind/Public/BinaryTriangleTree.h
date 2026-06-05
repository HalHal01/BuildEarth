// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
class WORLDWIND_API BinaryTriangle
{

	
public:
	BinaryTriangle(short int vertice1, short int vertice2, short int vertice3);
public:
	short int i1, i2, i3; // indices to _elevatedVertices
	short int lc, rc;	 // indices to left and right childs in _treeList
	short int ln, rn, bn; // indices of left, right and bottom neighbours in _treeList
};


/**
 * 
 */
class WORLDWIND_API BinaryTriangleTree
{
public:
	BinaryTriangleTree();
	~BinaryTriangleTree();
};


