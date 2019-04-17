#pragma once


int leftBounds = -125,
rightBounds = 125,
topBounds = 100,
bottomBounds = -100;



class Rectangle
{
public:
	float x;
	float y;
	float width;
	float height;

	Rectangle(float x, float y, float width, float height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	
	}

};

//used to store data pertaining to rendered squares
struct SquareEntity {
	int id = 0;

	float speed = 0.0f;
	glm::vec2 position;
	glm::vec2 direction;
	GLuint color;

	float width = 1;
	float height = 1;

};

class Quadtree {

public:
	Quadtree(int level, Rectangle* bounds) {
		this->level = level;
		this->bounds = bounds;
	}

	~Quadtree(){
		Clear();
		delete bounds;
	}
	//Clears quadtree
	void Clear() {
		std::vector<SquareEntity>().swap(entities);
		for (Quadtree* node : nodes) {
			delete node;
		}
		nodes.clear();
	}

	void Split() {

		float sub_width = bounds->width / 2.0f;
		float sub_height = bounds->height / 2.0f;

		//Split into 4 quadrants by creating 4 subnodes. We do so by dividing the current node into 4 equal parts, giving each new subnode new bounds
		nodes.push_back(new Quadtree(this->level + 1, new Rectangle(this->bounds->x + sub_width, this->bounds->y, sub_width, sub_height)));
		nodes.push_back(new Quadtree(this->level + 1, new Rectangle(this->bounds->x, this->bounds->y, sub_width, sub_height)));
		nodes.push_back(new Quadtree(this->level + 1, new Rectangle(this->bounds->x, this->bounds->y + sub_height, sub_width, sub_height)));
		nodes.push_back(new Quadtree(this->level + 1, new Rectangle(this->bounds->x + sub_width, this->bounds->y + sub_height, sub_width, sub_height)));
	}

	/*
	  Determine which node the object belongs to. -1 means object cannot completely fit within a child node and is part of the parent node
	*/
	int GetIndex(SquareEntity entity)
	{
		int index = -1;

		double vertical_midpoint = bounds->x + (bounds->width / 2.0f);
		double horizontal_midpoint = bounds->y + (bounds->height / 2.0f);

		bool inLeft   =		(entity.position.x < vertical_midpoint && entity.position.x + entity.width < vertical_midpoint);
		bool inRight  =		(entity.position.x >= vertical_midpoint && entity.position.x + entity.width >= vertical_midpoint);
		bool inTop    =		(entity.position.y < horizontal_midpoint && entity.position.y + entity.height < horizontal_midpoint);
		bool inBottom =		(entity.position.y >= horizontal_midpoint && entity.position.y + entity.height >= horizontal_midpoint);

		if (inLeft)
		{
			if (inTop) index = 0;
			else if (inBottom) index = 3;
		}
		else if (inRight)
		{
			if (inTop) index = 1;
			else if (inBottom) index = 2;
		}

		return index;
	}


	void Insert(SquareEntity entity)
	{
		if (this->nodes.size() > 0 && this->nodes[0] != NULL)
		{
			int index = this->GetIndex(entity);

			if (index != -1)
			{
				this->nodes[index]->Insert(entity);
				return;
			}
		}

		this->entities.push_back(entity);

		if (this->entities.size() > this->MAX_ENTITIES && this->level < this->MAXIMUM_LEVELS)
		{
			if (this->nodes.size() == 0 || this->nodes[0] == NULL)
				this->Split();

			int i = 0;
			while (i < this->entities.size())
			{
				int index = this->GetIndex(this->entities[i]);
				if (index != -1)
				{
					this->nodes[index]->Insert(this->entities[i]);
					this->entities.erase(this->entities.begin() + i);
				}
				else i++;
			}
		}
	}


	void Retrieve(std::vector<SquareEntity> *returnedEntities, SquareEntity entity)
	{
		int index = GetIndex(entity);
		if (index != -1 && nodes.size() != 0) nodes[index]->Retrieve(returnedEntities, entity);
		returnedEntities->insert(returnedEntities->end(), entities.begin(), entities.end());
	}

private:
	const int MAX_ENTITIES = 4; //maximum objects a node can contain before splitting    3-6
	const int MAXIMUM_LEVELS = 8;	//constraint: deepest level a subnode can be

	int level; //current level
	Rectangle* bounds;
	std::vector<SquareEntity> entities = {}; //entities within this node
	std::vector<Quadtree*> nodes = {};

};
