#include "shared.h"
#include "World.h"
#include "Scripting/Scripting.h"
void Cle::Components::PhysicsComponent::onLoad(Transform& tr)
{
	t = &tr;
	if (body) return;

	auto& h = Cle::ScriptHandler::getInstance();
	physicsCommon = &h.world->physicsCommon;
	physicsWorld = h.world->physicsWorld;

	body = physicsWorld->createRigidBody(reactphysics3d::Transform(
		glmtorp3dvec3(t->getPosition()), glmtorp3dquat(t->getOrientation())));

	if (name == reactphysics3d::CollisionShapeName::BOX)
		body->addCollider(physicsCommon->createBoxShape(glmtorp3dvec3(t->getScale()) / 2.0f),
			reactphysics3d::Transform::identity());

	body->setType(anchored ? reactphysics3d::BodyType::STATIC : reactphysics3d::BodyType::DYNAMIC);
	setVelocity(pendingVel);
	body->updateLocalInertiaTensorFromColliders();

}
Cle::Components::PhysicsComponent::PhysicsComponent(Transform& tr, reactphysics3d::CollisionShapeName n, bool isAnchored)
	: anchored(isAnchored), name(n)
{
	t = &tr;

	Cle::World* world = Cle::ScriptHandler::getInstance().world;
	if (!world)
	{
		std::cout << "no world for physics\n";
		return;
	}
	if (name == reactphysics3d::CollisionShapeName::BOX)
	{
		body = world->physicsWorld->createRigidBody(reactphysics3d::Transform(glmtorp3dvec3(t->getPosition()), glmtorp3dquat(t->getOrientation())));
		body->addCollider(world->physicsCommon.createBoxShape(glmtorp3dvec3(t->getScale()) / 2.0f), reactphysics3d::Transform::identity());

		body->setType(isAnchored ? reactphysics3d::BodyType::STATIC : reactphysics3d::BodyType::DYNAMIC);

	}
	body->updateLocalInertiaTensorFromColliders();

}