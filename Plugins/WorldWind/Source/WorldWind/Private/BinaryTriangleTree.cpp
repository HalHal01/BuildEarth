// Fill out your copyright notice in the Description page of Project Settings.


#include "BinaryTriangleTree.h"

BinaryTriangle::BinaryTriangle(short int vertice1, short int vertice2, short int vertice3)
{
	i1 = vertice1;
	i2 = vertice2;
	i3 = vertice3;
	lc = -1; // no left/right child
	rc = -1;
	ln = -1; // no neighbours
	rn = -1;
	bn = -1;
}

BinaryTriangleTree::BinaryTriangleTree()
{
}

BinaryTriangleTree::~BinaryTriangleTree()
{
}
