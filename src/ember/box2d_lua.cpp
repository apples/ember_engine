#include "box2d_helpers.hpp"

#include "utility.hpp"

#include <sol.hpp>

namespace ember {

void box2d_lua_enable(sol::state& lua) {
    if (lua["ember_box2d_enabled"]) {
        return;
    } else {
        lua["ember_box2d_enabled"] = true;
    }

    auto b2Vec2_ut = lua.new_usertype<b2Vec2>("b2Vec2", sol::constructors<b2Vec2(), b2Vec2(float, float)>{});
    b2Vec2_ut["IsValid"] = &b2Vec2::IsValid;
    b2Vec2_ut["Length"] = &b2Vec2::Length;
    b2Vec2_ut["LengthSquared"] = &b2Vec2::LengthSquared;
    b2Vec2_ut["Normalize"] = &b2Vec2::Normalize;
    b2Vec2_ut["Set"] = &b2Vec2::Set;
    b2Vec2_ut["SetZero"] = &b2Vec2::SetZero;
    b2Vec2_ut["Skew"] = &b2Vec2::Skew;
    b2Vec2_ut["x"] = &b2Vec2::x;
    b2Vec2_ut["y"] = &b2Vec2::y;
    b2Vec2_ut[sol::meta_function::addition] = [](const b2Vec2& a, const b2Vec2& b){
        auto x = a;
        x += b;
        return x;
    };
    b2Vec2_ut[sol::meta_function::subtraction] = [](const b2Vec2& a, const b2Vec2& b){
        auto x = a;
        x -= b;
        return x;
    };
    b2Vec2_ut[sol::meta_function::multiplication] = sol::overload(
        [](const b2Vec2& a, float b){
            auto x = a;
            x *= b;
            return x;
        },
        [](float a, const b2Vec2& b){
            auto x = b;
            x *= a;
            return x;
        });
    
    auto b2Rot_ut = lua.new_usertype<b2Rot>("b2Rot", sol::constructors<b2Rot(), b2Rot(float)>{});
    b2Rot_ut["GetAngle"] = &b2Rot::GetAngle;
    b2Rot_ut["GetXAxis"] = &b2Rot::GetXAxis;
    b2Rot_ut["GetYAxis"] = &b2Rot::GetYAxis;
    b2Rot_ut["Set"] = &b2Rot::Set;
    b2Rot_ut["SetIdentity"] = &b2Rot::SetIdentity;
    b2Rot_ut["c"] = &b2Rot::c;
    b2Rot_ut["s"] = &b2Rot::s;

    auto b2Transform_ut = lua.new_usertype<b2Transform>(
        "b2Transform", sol::constructors<b2Transform(), b2Transform(const b2Vec2&, const b2Rot&)>{});
    b2Transform_ut["Set"] = &b2Transform::Set;
    b2Transform_ut["SetIdentity"] = &b2Transform::SetIdentity;
    b2Transform_ut["p"] = &b2Transform::p;
    b2Transform_ut["q"] = &b2Transform::q;

    auto b2World_ut = lua.new_usertype<b2World>("b2World", sol::constructors<>{});
    b2World_ut["ClearForces"] = &b2World::ClearForces;
    b2World_ut["CreateBody"] = &b2World::CreateBody;
    b2World_ut["CreateJoint"] = &b2World::CreateJoint;
    b2World_ut["DebugDraw"] = &b2World::DebugDraw;
    b2World_ut["DestroyBody"] = &b2World::DestroyBody;
    b2World_ut["DestroyJoint"] = &b2World::DestroyJoint;
    b2World_ut["Dump"] = &b2World::Dump;
    b2World_ut["GetAllowSleeping"] = &b2World::GetAllowSleeping;
    b2World_ut["GetAutoClearForces"] = &b2World::GetAutoClearForces;
    b2World_ut["GetBodyCount"] = &b2World::GetBodyCount;
    b2World_ut["GetBodyList"] = static_cast<b2Body* (b2World::*)()>(&b2World::GetBodyList);
    b2World_ut["GetContactCount"] = &b2World::GetContactCount;
    b2World_ut["GetContactList"] = static_cast<b2Contact* (b2World::*)()>(&b2World::GetContactList);
    b2World_ut["GetContactManager"] = &b2World::GetContactManager;
    b2World_ut["GetContinuousPhysics"] = &b2World::GetContinuousPhysics;
    b2World_ut["GetGravity"] = &b2World::GetGravity;
    b2World_ut["GetJointCount"] = &b2World::GetJointCount;
    b2World_ut["GetJointList"] = static_cast<b2Joint* (b2World::*)()>(&b2World::GetJointList);
    b2World_ut["GetProfile"] = &b2World::GetProfile;
    b2World_ut["GetProxyCount"] = &b2World::GetProxyCount;
    b2World_ut["GetSubStepping"] = &b2World::GetSubStepping;
    b2World_ut["GetTreeBalance"] = &b2World::GetTreeBalance;
    b2World_ut["GetTreeHeight"] = &b2World::GetTreeHeight;
    b2World_ut["GetTreeQuality"] = &b2World::GetTreeQuality;
    b2World_ut["GetWarmStarting"] = &b2World::GetWarmStarting;
    b2World_ut["IsLocked"] = &b2World::IsLocked;
    b2World_ut["QueryAABB"] = &b2World::QueryAABB;
    b2World_ut["RayCast"] = &b2World::RayCast;
    b2World_ut["SetAllowSleeping"] = &b2World::SetAllowSleeping;
    b2World_ut["SetAutoClearForces"] = &b2World::SetAutoClearForces;
    b2World_ut["SetContactFilter"] = &b2World::SetContactFilter;
    b2World_ut["SetContactListener"] = &b2World::SetContactListener;
    b2World_ut["SetContinuousPhysics"] = &b2World::SetContinuousPhysics;
    b2World_ut["SetDebugDraw"] = &b2World::SetDebugDraw;
    b2World_ut["SetDestructionListener"] = &b2World::SetDestructionListener;
    b2World_ut["SetGravity"] = &b2World::SetGravity;
    b2World_ut["SetSubStepping"] = &b2World::SetSubStepping;
    b2World_ut["SetWarmStarting"] = &b2World::SetWarmStarting;
    b2World_ut["ShiftOrigin"] = &b2World::ShiftOrigin;
    b2World_ut["Step"] = &b2World::Step;

    auto b2Body_ut = lua.new_usertype<b2Body>("b2Body", sol::constructors<>{});
    b2Body_ut["ApplyAngularImpulse"] = &b2Body::ApplyAngularImpulse;
    b2Body_ut["ApplyForce"] = &b2Body::ApplyForce;
    b2Body_ut["ApplyForceToCenter"] = &b2Body::ApplyForceToCenter;
    b2Body_ut["ApplyLinearImpulse"] = &b2Body::ApplyLinearImpulse;
    b2Body_ut["ApplyLinearImpulseToCenter"] = &b2Body::ApplyLinearImpulseToCenter;
    b2Body_ut["ApplyTorque"] = &b2Body::ApplyTorque;
    b2Body_ut["CreateFixture"] = sol::overload(
        static_cast<b2Fixture* (b2Body::*)(const b2FixtureDef*)>(&b2Body::CreateFixture),
        static_cast<b2Fixture* (b2Body::*)(const b2Shape*, float)>(&b2Body::CreateFixture));
    b2Body_ut["DestroyFixture"] = &b2Body::DestroyFixture;
    b2Body_ut["Dump"] = &b2Body::Dump;
    b2Body_ut["GetAngle"] = &b2Body::GetAngle;
    b2Body_ut["GetAngularDamping"] = &b2Body::GetAngularDamping;
    b2Body_ut["GetAngularVelocity"] = &b2Body::GetAngularVelocity;
    b2Body_ut["GetContactList"] = static_cast<b2ContactEdge* (b2Body::*)()>(&b2Body::GetContactList);
    b2Body_ut["GetFixtureList"] = static_cast<b2Fixture* (b2Body::*)()>(&b2Body::GetFixtureList);
    b2Body_ut["GetGravityScale"] = &b2Body::GetGravityScale;
    b2Body_ut["GetInertia"] = &b2Body::GetInertia;
    b2Body_ut["GetJointList"] = static_cast<b2JointEdge* (b2Body::*)()>(&b2Body::GetJointList);
    b2Body_ut["GetLinearDamping"] = &b2Body::GetLinearDamping;
    b2Body_ut["GetLinearVelocity"] = &b2Body::GetLinearVelocity;
    b2Body_ut["GetLinearVelocityFromLocalPoint"] = &b2Body::GetLinearVelocityFromLocalPoint;
    b2Body_ut["GetLinearVelocityFromWorldPoint"] = &b2Body::GetLinearVelocityFromWorldPoint;
    b2Body_ut["GetLocalCenter"] = &b2Body::GetLocalCenter;
    b2Body_ut["GetLocalPoint"] = &b2Body::GetLocalPoint;
    b2Body_ut["GetLocalVector"] = &b2Body::GetLocalVector;
    b2Body_ut["GetMass"] = &b2Body::GetMass;
    b2Body_ut["GetMassData"] = &b2Body::GetMassData;
    b2Body_ut["GetNext"] = static_cast<b2Body* (b2Body::*)()>(&b2Body::GetNext);
    b2Body_ut["GetPosition"] = &b2Body::GetPosition;
    b2Body_ut["GetTransform"] = &b2Body::GetTransform;
    b2Body_ut["GetType"] = &b2Body::GetType;
    b2Body_ut["GetUserData"] = &b2Body::GetUserData;
    b2Body_ut["GetWorld"] = static_cast<b2World* (b2Body::*)()>(&b2Body::GetWorld);
    b2Body_ut["GetWorldCenter"] = &b2Body::GetWorldCenter;
    b2Body_ut["GetWorldPoint"] = &b2Body::GetWorldPoint;
    b2Body_ut["GetWorldVector"] = &b2Body::GetWorldVector;
    b2Body_ut["IsAwake"] = &b2Body::IsAwake;
    b2Body_ut["IsBullet"] = &b2Body::IsBullet;
    b2Body_ut["IsEnabled"] = &b2Body::IsEnabled;
    b2Body_ut["IsFixedRotation"] = &b2Body::IsFixedRotation;
    b2Body_ut["IsSleepingAllowed"] = &b2Body::IsSleepingAllowed;
    b2Body_ut["ResetMassData"] = &b2Body::ResetMassData;
    b2Body_ut["SetAngularDamping"] = &b2Body::SetAngularDamping;
    b2Body_ut["SetAngularVelocity"] = &b2Body::SetAngularVelocity;
    b2Body_ut["SetAwake"] = &b2Body::SetAwake;
    b2Body_ut["SetBullet"] = &b2Body::SetBullet;
    b2Body_ut["SetEnabled"] = &b2Body::SetEnabled;
    b2Body_ut["SetFixedRotation"] = &b2Body::SetFixedRotation;
    b2Body_ut["SetGravityScale"] = &b2Body::SetGravityScale;
    b2Body_ut["SetLinearDamping"] = &b2Body::SetLinearDamping;
    b2Body_ut["SetLinearVelocity"] = &b2Body::SetLinearVelocity;
    b2Body_ut["SetMassData"] = &b2Body::SetMassData;
    b2Body_ut["SetSleepingAllowed"] = &b2Body::SetSleepingAllowed;
    b2Body_ut["SetTransform"] = &b2Body::SetTransform;
    b2Body_ut["SetType"] = &b2Body::SetType;
    b2Body_ut["SetUserData"] = &b2Body::SetUserData;

    auto b2BodyDef_ut = lua.new_usertype<b2BodyDef>("b2BodyDef", sol::constructors<b2BodyDef()>{});
    b2BodyDef_ut["allowSleep"] = &b2BodyDef::allowSleep;
    b2BodyDef_ut["angle"] = &b2BodyDef::angle;
    b2BodyDef_ut["angularDamping"] = &b2BodyDef::angularDamping;
    b2BodyDef_ut["angularVelocity"] = &b2BodyDef::angularVelocity;
    b2BodyDef_ut["awake"] = &b2BodyDef::awake;
    b2BodyDef_ut["bullet"] = &b2BodyDef::bullet;
    b2BodyDef_ut["enabled"] = &b2BodyDef::enabled;
    b2BodyDef_ut["fixedRotation"] = &b2BodyDef::fixedRotation;
    b2BodyDef_ut["gravityScale"] = &b2BodyDef::gravityScale;
    b2BodyDef_ut["linearDamping"] = &b2BodyDef::linearDamping;
    b2BodyDef_ut["linearVelocity"] = &b2BodyDef::linearVelocity;
    b2BodyDef_ut["position"] = &b2BodyDef::position;
    b2BodyDef_ut["type"] = &b2BodyDef::type;
    b2BodyDef_ut["userData"] = &b2BodyDef::userData;

    lua.new_enum<b2BodyType>(
        "b2BodyType",
        {
            {"b2_staticBody", b2BodyType::b2_staticBody},
            {"b2_kinematicBody", b2BodyType::b2_kinematicBody},
            {"b2_dynamicBody", b2BodyType::b2_dynamicBody},
        });
    
    auto b2Fixture_ut = lua.new_usertype<b2Fixture>("b2Fixture", sol::constructors<>{});
    b2Fixture_ut["Dump"] = &b2Fixture::Dump;
    b2Fixture_ut["GetAABB"] = &b2Fixture::GetAABB;
    b2Fixture_ut["GetBody"] = static_cast<b2Body* (b2Fixture::*)()>(&b2Fixture::GetBody);
    b2Fixture_ut["GetDensity"] = &b2Fixture::GetDensity;
    b2Fixture_ut["GetFilterData"] = &b2Fixture::GetFilterData;
    b2Fixture_ut["GetFriction"] = &b2Fixture::GetFriction;
    b2Fixture_ut["GetMassData"] = &b2Fixture::GetMassData;
    b2Fixture_ut["GetNext"] = static_cast<b2Fixture* (b2Fixture::*)()>(&b2Fixture::GetNext);
    b2Fixture_ut["GetRestitution"] = &b2Fixture::GetRestitution;
    b2Fixture_ut["GetShape"] = static_cast<b2Shape* (b2Fixture::*)()>(&b2Fixture::GetShape);
    b2Fixture_ut["GetType"] = &b2Fixture::GetType;
    b2Fixture_ut["GetUserData"] = &b2Fixture::GetUserData;
    b2Fixture_ut["IsSensor"] = &b2Fixture::IsSensor;
    b2Fixture_ut["RayCast"] = &b2Fixture::RayCast;
    b2Fixture_ut["Refilter"] = &b2Fixture::Refilter;
    b2Fixture_ut["SetDensity"] = &b2Fixture::SetDensity;
    b2Fixture_ut["SetFilterData"] = &b2Fixture::SetFilterData;
    b2Fixture_ut["SetFriction"] = &b2Fixture::SetFriction;
    b2Fixture_ut["SetRestitution"] = &b2Fixture::SetRestitution;
    b2Fixture_ut["SetSensor"] = &b2Fixture::SetSensor;
    b2Fixture_ut["SetUserData"] = &b2Fixture::SetUserData;
    b2Fixture_ut["TestPoint"] = &b2Fixture::TestPoint;

    auto b2FixtureDef_ut = lua.new_usertype<b2FixtureDef>("b2FixtureDef", sol::constructors<b2FixtureDef()>{});
    b2FixtureDef_ut["density"] = &b2FixtureDef::density;
    b2FixtureDef_ut["filter"] = &b2FixtureDef::filter;
    b2FixtureDef_ut["friction"] = &b2FixtureDef::friction;
    b2FixtureDef_ut["isSensor"] = &b2FixtureDef::isSensor;
    b2FixtureDef_ut["restitution"] = &b2FixtureDef::restitution;
    b2FixtureDef_ut["shape"] = &b2FixtureDef::shape;
    b2FixtureDef_ut["userData"] = &b2FixtureDef::userData;

    auto b2Shape_ut = lua.new_usertype<b2Shape>("b2Shape", sol::constructors<>{});
    b2Shape_ut["Clone"] = &b2Shape::Clone;
    b2Shape_ut["ComputeAABB"] = &b2Shape::ComputeAABB;
    b2Shape_ut["ComputeMass"] = &b2Shape::ComputeMass;
    b2Shape_ut["GetChildCount"] = &b2Shape::GetChildCount;
    b2Shape_ut["GetType"] = &b2Shape::GetType;
    b2Shape_ut["RayCast"] = &b2Shape::RayCast;
    b2Shape_ut["TestPoint"] = &b2Shape::TestPoint;
    b2Shape_ut["m_type"] = &b2Shape::m_type;
    b2Shape_ut["m_radius"] = &b2Shape::m_radius;

    b2Shape_ut["Type"] = lua.create_table();
    b2Shape_ut["Type"]["e_circle"] = b2Shape::Type::e_circle;
    b2Shape_ut["Type"]["e_edge"] = b2Shape::Type::e_edge;
    b2Shape_ut["Type"]["e_polygon"] = b2Shape::Type::e_polygon;
    b2Shape_ut["Type"]["e_chain"] = b2Shape::Type::e_chain;
    b2Shape_ut["Type"]["e_typeCount"] = b2Shape::Type::e_typeCount;

    auto b2CircleShape_ut = lua.new_usertype<b2CircleShape>(
        "b2CircleShape", sol::constructors<b2CircleShape()>{}, sol::base_classes, sol::bases<b2Shape>{});
    b2CircleShape_ut["Clone"] = &b2CircleShape::Clone;
    b2CircleShape_ut["ComputeAABB"] = &b2CircleShape::ComputeAABB;
    b2CircleShape_ut["ComputeMass"] = &b2CircleShape::ComputeMass;
    b2CircleShape_ut["GetChildCount"] = &b2CircleShape::GetChildCount;
    b2CircleShape_ut["GetType"] = &b2CircleShape::GetType;
    b2CircleShape_ut["RayCast"] = &b2CircleShape::RayCast;
    b2CircleShape_ut["TestPoint"] = &b2CircleShape::TestPoint;
    b2CircleShape_ut["m_type"] = &b2CircleShape::m_type;
    b2CircleShape_ut["m_radius"] = &b2CircleShape::m_radius;
    b2CircleShape_ut["m_p"] = &b2CircleShape::m_p;

    auto b2EdgeShape_ut = lua.new_usertype<b2EdgeShape>(
        "b2EdgeShape", sol::constructors<b2EdgeShape()>{}, sol::base_classes, sol::bases<b2Shape>{});
    b2EdgeShape_ut["Clone"] = &b2EdgeShape::Clone;
    b2EdgeShape_ut["ComputeAABB"] = &b2EdgeShape::ComputeAABB;
    b2EdgeShape_ut["ComputeMass"] = &b2EdgeShape::ComputeMass;
    b2EdgeShape_ut["GetChildCount"] = &b2EdgeShape::GetChildCount;
    b2EdgeShape_ut["GetType"] = &b2EdgeShape::GetType;
    b2EdgeShape_ut["RayCast"] = &b2EdgeShape::RayCast;
    b2EdgeShape_ut["TestPoint"] = &b2EdgeShape::TestPoint;
    b2EdgeShape_ut["m_type"] = &b2EdgeShape::m_type;
    b2EdgeShape_ut["m_radius"] = &b2EdgeShape::m_radius;
    b2EdgeShape_ut["SetOneSided"] = &b2EdgeShape::SetOneSided;
    b2EdgeShape_ut["SetTwoSided"] = &b2EdgeShape::SetTwoSided;
    b2EdgeShape_ut["m_oneSided"] = &b2EdgeShape::m_oneSided;
    b2EdgeShape_ut["m_vertex0"] = &b2EdgeShape::m_vertex0;
    b2EdgeShape_ut["m_vertex1"] = &b2EdgeShape::m_vertex1;
    b2EdgeShape_ut["m_vertex2"] = &b2EdgeShape::m_vertex2;
    b2EdgeShape_ut["m_vertex3"] = &b2EdgeShape::m_vertex3;

    auto b2PolygonShape_ut = lua.new_usertype<b2PolygonShape>(
        "b2PolygonShape", sol::constructors<b2PolygonShape()>{}, sol::base_classes, sol::bases<b2Shape>{});
    b2PolygonShape_ut["Clone"] = &b2PolygonShape::Clone;
    b2PolygonShape_ut["ComputeAABB"] = &b2PolygonShape::ComputeAABB;
    b2PolygonShape_ut["ComputeMass"] = &b2PolygonShape::ComputeMass;
    b2PolygonShape_ut["GetChildCount"] = &b2PolygonShape::GetChildCount;
    b2PolygonShape_ut["GetType"] = &b2PolygonShape::GetType;
    b2PolygonShape_ut["RayCast"] = &b2PolygonShape::RayCast;
    b2PolygonShape_ut["TestPoint"] = &b2PolygonShape::TestPoint;
    b2PolygonShape_ut["m_type"] = &b2PolygonShape::m_type;
    b2PolygonShape_ut["m_radius"] = &b2PolygonShape::m_radius;
    b2PolygonShape_ut["Set"] = [](b2PolygonShape& s, const sol::table& a) {
        b2Vec2 p[b2_maxPolygonVertices];
        int c = a.size();
        for (int i = 1; i <= c; ++i) {
            p[i-1] = a[i];
        }
        s.Set(p, c);
    };
    b2PolygonShape_ut["SetAsBox"] = sol::overload(
        static_cast<void (b2PolygonShape::*)(float, float)>(&b2PolygonShape::SetAsBox),
        static_cast<void (b2PolygonShape::*)(float, float, const b2Vec2&, float)>(&b2PolygonShape::SetAsBox));
    b2PolygonShape_ut["Validate"] = &b2PolygonShape::Validate;
    b2PolygonShape_ut["m_centroid"] = &b2PolygonShape::m_centroid;
    b2PolygonShape_ut["m_count"] = &b2PolygonShape::m_count;
    b2PolygonShape_ut["m_normals"] = &b2PolygonShape::m_normals;
    b2PolygonShape_ut["m_vertices"] = &b2PolygonShape::m_vertices;

    auto b2ChainShape_ut = lua.new_usertype<b2ChainShape>(
        "b2ChainShape", sol::constructors<b2ChainShape()>{}, sol::base_classes, sol::bases<b2Shape>{});
    b2ChainShape_ut["Clone"] = &b2ChainShape::Clone;
    b2ChainShape_ut["ComputeAABB"] = &b2ChainShape::ComputeAABB;
    b2ChainShape_ut["ComputeMass"] = &b2ChainShape::ComputeMass;
    b2ChainShape_ut["GetChildCount"] = &b2ChainShape::GetChildCount;
    b2ChainShape_ut["GetType"] = &b2ChainShape::GetType;
    b2ChainShape_ut["RayCast"] = &b2ChainShape::RayCast;
    b2ChainShape_ut["TestPoint"] = &b2ChainShape::TestPoint;
    b2ChainShape_ut["m_type"] = &b2ChainShape::m_type;
    b2ChainShape_ut["m_radius"] = &b2ChainShape::m_radius;
    b2ChainShape_ut["Clear"] = &b2ChainShape::Clear;
    b2ChainShape_ut["CreateChain"] = [](b2ChainShape& s, const sol::table& a, const b2Vec2& prev, const b2Vec2& next){
        auto points = std::vector<b2Vec2>{};
        auto count = a.size();
        points.reserve(count);
        for (int i = 1; i <= count; ++i) {
            points.push_back(a[i]);
        }
        s.CreateChain(points.data(), count, prev, next);
    };
    b2ChainShape_ut["CreateLoop"] = [](b2ChainShape& s, const sol::table& a) {
        auto points = std::vector<b2Vec2>{};
        auto count = a.size();
        points.reserve(count);
        for (int i = 1; i <= count; ++i) {
            points.push_back(a[i]);
        }
        s.CreateLoop(points.data(), count);
    };
    b2ChainShape_ut["GetChildEdge"] = &b2ChainShape::GetChildEdge;
    b2ChainShape_ut["m_count"] = &b2ChainShape::m_count;
    b2ChainShape_ut["m_nextVertex"] = &b2ChainShape::m_nextVertex;
    b2ChainShape_ut["m_prevVertex"] = &b2ChainShape::m_prevVertex;
    b2ChainShape_ut["m_vertices"] = sol::readonly_property([](const b2ChainShape& s) {
        return utility::span{s.m_vertices, std::size_t(s.m_count)};
    });

    auto b2Contact_ut = lua.new_usertype<b2Contact>("b2Contact", sol::constructors<>{});
    b2Contact_ut["Evaluate"] = &b2Contact::Evaluate;
    b2Contact_ut["GetChildIndexA"] = &b2Contact::GetChildIndexA;
    b2Contact_ut["GetChildIndexB"] = &b2Contact::GetChildIndexB;
    b2Contact_ut["GetFixtureA"] = static_cast<b2Fixture* (b2Contact::*)()>(&b2Contact::GetFixtureA);
    b2Contact_ut["GetFixtureB"] = static_cast<b2Fixture* (b2Contact::*)()>(&b2Contact::GetFixtureB);
    b2Contact_ut["GetFriction"] = &b2Contact::GetFriction;
    b2Contact_ut["GetManifold"] = static_cast<b2Manifold* (b2Contact::*)()>(&b2Contact::GetManifold);
    b2Contact_ut["GetNext"] = static_cast<b2Contact* (b2Contact::*)()>(&b2Contact::GetNext);
    b2Contact_ut["GetRestitution"] = &b2Contact::GetRestitution;
    b2Contact_ut["GetTangentSpeed"] = &b2Contact::GetTangentSpeed;
    b2Contact_ut["GetWorldManifold"] = &b2Contact::GetWorldManifold;
    b2Contact_ut["IsEnabled"] = &b2Contact::IsEnabled;
    b2Contact_ut["IsTouching"] = &b2Contact::IsTouching;
    b2Contact_ut["ResetFriction"] = &b2Contact::ResetFriction;
    b2Contact_ut["ResetRestitution"] = &b2Contact::ResetRestitution;
    b2Contact_ut["SetEnabled"] = &b2Contact::SetEnabled;
    b2Contact_ut["SetFriction"] = &b2Contact::SetFriction;
    b2Contact_ut["SetRestitution"] = &b2Contact::SetRestitution;
    b2Contact_ut["SetTangentSpeed"] = &b2Contact::SetTangentSpeed;

    auto b2Manifold_ut = lua.new_usertype<b2Manifold>("b2Manifold", sol::constructors<b2Manifold()>{});
    b2Manifold_ut["localNormal"] = &b2Manifold::localNormal;
    b2Manifold_ut["localPoint"] = &b2Manifold::localPoint;
    b2Manifold_ut["pointCount"] = &b2Manifold::pointCount;
    b2Manifold_ut["points"] = &b2Manifold::points;
    b2Manifold_ut["type"] = &b2Manifold::type;
    b2Manifold_ut["Type"] = lua.create_table();
    b2Manifold_ut["Type"]["e_circles"] = b2Manifold::Type::e_circles;
    b2Manifold_ut["Type"]["e_faceA"] = b2Manifold::Type::e_faceA;
    b2Manifold_ut["Type"]["e_faceB"] = b2Manifold::Type::e_faceB;

    auto b2WorldManifold_ut =
        lua.new_usertype<b2WorldManifold>("b2WorldManifold", sol::constructors<b2WorldManifold()>{});
    b2WorldManifold_ut["Initialize"] = &b2WorldManifold::Initialize;
    b2WorldManifold_ut["normal"] = &b2WorldManifold::normal;
    b2WorldManifold_ut["points"] = &b2WorldManifold::points;
    b2WorldManifold_ut["separations"] = &b2WorldManifold::separations;

    auto b2ContactEdge_ut = lua.new_usertype<b2ContactEdge>("b2ContactEdge", sol::constructors<>{});
    b2ContactEdge_ut["contact"] = &b2ContactEdge::contact;
    b2ContactEdge_ut["next"] = &b2ContactEdge::next;
    b2ContactEdge_ut["other"] = &b2ContactEdge::other;
    b2ContactEdge_ut["prev"] = &b2ContactEdge::prev;

    auto b2ContactImpulse_ut = lua.new_usertype<b2ContactImpulse>("b2ContactImpulse", sol::constructors<>{});
    b2ContactImpulse_ut["count"] = &b2ContactImpulse::count;
    b2ContactImpulse_ut["normalImpulses"] = &b2ContactImpulse::normalImpulses;
    b2ContactImpulse_ut["tangentImpulses"] = &b2ContactImpulse::tangentImpulses;
}

} // namespace ember
