
#include "Octree.h"
#include "IPhysicsObject.h"
#include "ICollisionMesh.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

namespace ginkgo
{
	float getTop(ICollisionMesh const& mesh);
	float getBot(ICollisionMesh const& mesh);
	float getLeft(ICollisionMesh const& mesh);
	float getRight(ICollisionMesh const& mesh);
	float getFront(ICollisionMesh const& mesh);
	float getBack(ICollisionMesh const& mesh);


	Octree::Octree(int level, Prism const& bounds, Octree* parent)
	{
		this->level = level;
		objects.reserve(OCTREE_MAXENTS + 1);
		this->bounds = bounds;
		this->parent = parent;
		for (int a = 0; a < 8; a++)
		{
			leaves[a] = nullptr;
		}
	}

	void Octree::clear()
	{
		objects.clear();
		for (int a = 0; a < 8; a++)
		{
			if (leaves[a] != nullptr)
			{
				delete leaves[a];
				leaves[a] = nullptr;
			}
		}
	}

	void Octree::split()
	{
		float w = bounds.w / 2.f;
		float h = bounds.h / 2.f;
		float l = bounds.l / 2.f;

		if (leaves[0] == nullptr)
			leaves[0] = new Octree(level + 1, Prism(bounds.x, bounds.y, bounds.z, w, h, l), this);
		if (leaves[1] == nullptr)
			leaves[1] = new Octree(level + 1, Prism(bounds.x, bounds.y, bounds.z + l, w, h, l), this);
		if (leaves[2] == nullptr)
			leaves[2] = new Octree(level + 1, Prism(bounds.x, bounds.y + h, bounds.z + l, w, h, l), this);
		if (leaves[3] == nullptr)
			leaves[3] = new Octree(level + 1, Prism(bounds.x, bounds.y + h, bounds.z, w, h, l), this);
		if (leaves[4] == nullptr)
			leaves[4] = new Octree(level + 1, Prism(bounds.x + w, bounds.y, bounds.z, w, h, l), this);
		if (leaves[5] == nullptr)
			leaves[5] = new Octree(level + 1, Prism(bounds.x + w, bounds.y, bounds.z + l, w, h, l), this);
		if (leaves[6] == nullptr)
			leaves[6] = new Octree(level + 1, Prism(bounds.x + w, bounds.y + h, bounds.z + l, w, h, l), this);
		if (leaves[7] == nullptr)
			leaves[7] = new Octree(level + 1, Prism(bounds.x + w, bounds.y + h, bounds.z, w, h, l), this);
	}

	int Octree::getIndex(IPhysicsObject* object) const
	{
		int index = -1;
		float hfWidth = bounds.x + bounds.w / 2.f;
		float hfHeight = bounds.y + bounds.h / 2.f;
		float hfLength = bounds.z + bounds.l / 2.f;


		ICollisionMesh* mesh = object->getCollisionMesh();

		float top = getTop(*mesh);
		float bot = getBot(*mesh);
		float left = getLeft(*mesh);
		float right = getRight(*mesh);
		float front = getFront(*mesh);
		float back = getBack(*mesh);

		bool topHalf = (bot > hfHeight);
		bool botHalf = (top < hfHeight);

		bool leftHalf = (right < hfWidth);
		bool rightHalf = (left > hfWidth);

		if (back > hfLength)//z+
		{
			if (topHalf)//y+
			{
				if (leftHalf)//x-
				{
					index = 2;
				}
				else if (rightHalf)//x+
				{
					index = 6;
				}
			}
			else if (botHalf)//y-
			{
				if (leftHalf)//x-
				{
					index = 1;
				}
				else if (rightHalf)//x+
				{
					index = 5;
				}
			}
		}
		else if (front < hfLength)//z-
		{
			if (topHalf)//y+
			{
				if (leftHalf)//x-
				{
					index = 3;
				}
				else if (rightHalf)//x+
				{
					index = 7;
				}
			}
			else if (botHalf)//y-
			{
				if (leftHalf)//x-
				{
					index = 0;
				}
				else if (rightHalf)//x+
				{
					index = 4;
				}
			}
		}
		return index;
	}

	int Octree::getIndex(Ray const& ray, float dist) const
	{
		int index = -1;
		float hfWidth = bounds.x + bounds.w / 2.f;
		float hfHeight = bounds.y + bounds.h / 2.f;
		float hfLength = bounds.z + bounds.l / 2.f;

		glm::vec3 endpt = ray.point + (ray.direction * dist);

		float top = max(endpt.y, ray.point.y);
		float bot = min(endpt.y, ray.point.y);
		float left = min(endpt.x, ray.point.x);
		float right = max(endpt.x, ray.point.x);
		float front = max(endpt.z, ray.point.z);
		float back = min(endpt.z, ray.point.z);

		bool topHalf = (bot > hfHeight);
		bool botHalf = (top < hfHeight);

		bool leftHalf = (right < hfWidth);
		bool rightHalf = (left > hfWidth);

		//TODO: repeated code, refactor me

		if (back > hfLength)//z+
		{
			if (topHalf)//y+
			{
				if (leftHalf)//x-
				{
					index = 2;
				}
				else if (rightHalf)//x+
				{
					index = 6;
				}
			}
			else if (botHalf)//y-
			{
				if (leftHalf)//x-
				{
					index = 1;
				}
				else if (rightHalf)//x+
				{
					index = 5;
				}
			}
		}
		else if (front < hfLength)//z-
		{
			if (topHalf)//y+
			{
				if (leftHalf)//x-
				{
					index = 3;
				}
				else if (rightHalf)//x+
				{
					index = 7;
				}
			}
			else if (botHalf)//y-
			{
				if (leftHalf)//x-
				{
					index = 0;
				}
				else if (rightHalf)//x+
				{
					index = 4;
				}
			}
		}
		return index;
	}

	void Octree::insert(IPhysicsObject* object)
	{
		if (leaves[0] != nullptr)
		{
			int index = getIndex(object);
			if (index != -1)
			{
				leaves[index]->insert(object);
				return;
			}
		}

		objects.push_back(object);
		
		if (objects.size() > OCTREE_MAXENTS && level < OCTREE_MAXLEVELS)
		{
			if (leaves[0] == nullptr)
			{
				split();
			}

			UINT32 a = 0;
			while (a < objects.size())
			{
				int index = getIndex(objects[a]);
				if (index != -1)
				{
					leaves[index]->insert(objects[a]);
					objects.erase(objects.begin() + a);
				}
				else
				{
					a++;
				}
			}
		}
	}

	void Octree::retrieveCollisions(vector<IPhysicsObject*>& outObjects, IPhysicsObject* collider) const
	{
		int index = getIndex(collider);
		if (index != -1 && leaves[0] != nullptr)
		{
			leaves[index]->retrieveCollisions(outObjects, collider);
		}

		for (UINT32 a = 0; a < objects.size(); a++)
		{
			outObjects.push_back(objects[a]);
		}
	}

	void Octree::retrieveCollisions(vector<IPhysicsObject*>& outList, Ray const& ray, float dist) const
	{
		int index = getIndex(ray, dist);
		if (index != -1 && leaves[0] != nullptr)
		{
			leaves[index]->retrieveCollisions(outList, ray, dist);
		}

		for (UINT32 a = 0; a < objects.size(); a++)
		{
			outList.push_back(objects[a]);
		}
	}

	void Octree::resetTree(int level, Prism const& bounds)
	{
		if (parent != nullptr)
		{
			parent->resetTree(level, bounds);
			return;
		}

		clear();
		this->level = level;
		this->bounds = bounds;
		objects.reserve(OCTREE_MAXENTS + 1);
		parent = nullptr;
	}

	void Octree::fillTree(vector<IPhysicsObject*> const& objects)
	{
		for (IPhysicsObject* object : objects)
		{
			insert(object);
		}
	}

	int Octree::remove(IPhysicsObject const* object)
	{
		bool thisTree = false;
		UINT32 index;
		for (index = 0; index < objects.size(); index++)
		{
			if (objects[index]->getEntityID() == object->getEntityID())
			{
				thisTree = true;
			}
		}

		if (thisTree)
		{
			objects.erase(objects.begin() + index);
			if (objects.empty())
			{
				return REMOVE_FOUNDEMPTY;
			}
			return REMOVE_FOUNDNOTEMPTY;
		}
		else
		{
			for (int a = 0; a < 8; a++)
			{
				if (leaves[a] != nullptr)
				{
					int ret = leaves[a]->remove(object);
					switch (ret)
						{
						case REMOVE_FOUNDNOTEMPTY:
							return REMOVE_FOUNDBYCHILD;
						case REMOVE_FOUNDEMPTY:
						{
							bool allEmpty = true;
							for (int b = 0; b < 8; b++)
							{
								if (leaves[b] != nullptr)
								{
									if (!leaves[b]->empty())
									{
										allEmpty = false;
									}
								}
							}
							if (allEmpty)
							{
								return REMOVE_FOUNDEMPTY;
							}
							else
							{
								vector<IPhysicsObject*> leafEnts;
								leaves[a]->getChildLeaves(leafEnts);
								leaves[a]->clear();
								for (UINT32 b = 0; b < leafEnts.size(); b++)
								{
									leaves[a]->insert(leafEnts[b]);
								}
							}
							return REMOVE_FOUNDBYCHILD;
						}
						case REMOVE_FOUNDBYCHILD:
							return REMOVE_FOUNDBYCHILD;
					}
				}
			}
		}
		return REMOVE_NOTFOUND;
	}

	bool Octree::empty() const
	{
		return objects.empty();
	}

	void Octree::getChildLeaves(vector<IPhysicsObject*>& outList) const
	{
		for (int a = 0; a < 8; a++)
		{
			if (leaves[a] != nullptr)
			{
				leaves[a]->getChildLeaves(outList);
			}
		}

		for (UINT32 a = 0; a < objects.size(); a++)
		{
			outList.push_back(objects[a]);
		}
	}

	Prism const& Octree::getBounds() const
	{
		return bounds;
	}

	Octree::~Octree()
	{
		clear();
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//y+
	float getTop(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xY, yY, zY;
		xY = max(xAxis.y * mesh.getExtent(0), xAxis.y * -mesh.getExtent(0));
		yY = max(yAxis.y * mesh.getExtent(1), yAxis.y * -mesh.getExtent(1));
		zY = max(zAxis.y * mesh.getExtent(2), zAxis.y * -mesh.getExtent(2));

		return max(xY, max(yY, zY));
	}

	//y-
	float getBot(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xY, yY, zY;
		xY = min(xAxis.y * mesh.getExtent(0), xAxis.y * -mesh.getExtent(0));
		yY = min(yAxis.y * mesh.getExtent(1), yAxis.y * -mesh.getExtent(1));
		zY = min(zAxis.y * mesh.getExtent(2), zAxis.y * -mesh.getExtent(2));

		return min(xY, min(yY, zY));
	}

	//x-
	float getLeft(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xX, yX, zX;
		xX = min(xAxis.x * mesh.getExtent(0), xAxis.x * -mesh.getExtent(0));
		yX = min(yAxis.x * mesh.getExtent(1), yAxis.x * -mesh.getExtent(1));
		zX = min(zAxis.x * mesh.getExtent(2), zAxis.x * -mesh.getExtent(2));

		return min(xX, min(yX, zX));
	}

	//x+
	float getRight(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xX, yX, zX;
		xX = max(xAxis.x * mesh.getExtent(0), xAxis.x * -mesh.getExtent(0));
		yX = max(yAxis.x * mesh.getExtent(1), yAxis.x * -mesh.getExtent(1));
		zX = max(zAxis.x * mesh.getExtent(2), zAxis.x * -mesh.getExtent(2));

		return max(xX, max(yX, zX));
	}

	//z+
	float getFront(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xZ, yZ, zZ;
		xZ = max(xAxis.z * mesh.getExtent(0), xAxis.z * -mesh.getExtent(0));
		yZ = max(yAxis.z * mesh.getExtent(1), yAxis.z * -mesh.getExtent(1));
		zZ = max(zAxis.z * mesh.getExtent(2), zAxis.z * -mesh.getExtent(2));

		return max(xZ, max(yZ, zZ));
	}

	//z-
	float getBack(ICollisionMesh const& mesh)
	{
		glm::vec3 const& xAxis = mesh.getAxis(0);
		glm::vec3 const& yAxis = mesh.getAxis(1);
		glm::vec3 const& zAxis = mesh.getAxis(2);
		float xZ, yZ, zZ;
		xZ = min(xAxis.z * mesh.getExtent(0), xAxis.z * -mesh.getExtent(0));
		yZ = min(yAxis.z * mesh.getExtent(1), yAxis.z * -mesh.getExtent(1));
		zZ = min(zAxis.z * mesh.getExtent(2), zAxis.z * -mesh.getExtent(2));

		return min(xZ, min(yZ, zZ));
	}
}