/*
    @Author: Daniel Guerra Gallardo
    @program: Mechanism - 3D Animation
    @Date: 04/2021
    @Description: Desarollo mec�nico de primitivas animadas fisicamente en un entorno de renderizado 2D
*/

#include "Scene.h"

namespace Graphics
{

    Scene::Scene(b2World& _physics_world, int _scene_id) : physics_world (&_physics_world), scene_id(_scene_id)
    {
        scene_init();
    }

    void Scene::scene_init()
    {

        switch (scene_id)
        {
        case 0: 
            run_test_scene();
            break;
        case 1:
            run_exercise_scene();
            
        default:
            break;
        }
        
    }

    void Scene::run_test_scene()
    {
        Physics::Entity entity1(*physics_world, b2_staticBody, Body_Shape::Triangle, 2, 2, 0.5, 0.5);
        entity1.build_body();

        Physics::Entity entity2(*physics_world, b2_staticBody, Body_Shape::Polygon, 5, 2, 1, 0.5);
        entity2.build_body();

        Physics::Entity entity3(*physics_world, b2_dynamicBody, Body_Shape::Circle, 5, 5, 1, 1);
        entity3.build_body();
    }

    void Scene::run_exercise_scene()
    {
        // JUGADOR
        Physics::Entity player(*physics_world, b2_dynamicBody, Body_Shape::Circle, 2, 4, 0.25, 0.25);

        // PLATAFORMAS
        Physics::Entity platform_1(*physics_world, b2_kinematicBody, Body_Shape::Polygon, 11.65f, -0.75f, 1.25f, 0.20f);
        Physics::Entity platform_2(*physics_world, b2_kinematicBody, Body_Shape::Polygon, 4.5f, 2.5f, 1.10f, 0.20f);

        // ESCENARIO
        // --------- Piso Inferior ----------------

        // Est�ticos (Izquierda a derecha)
        Physics::Entity floor_1(*physics_world, b2_staticBody, Body_Shape::Polygon, 2.20f, -0.2, 1.1f, 0.75f);
        Physics::Entity floor_2(*physics_world, b2_staticBody, Body_Shape::Polygon, 6.85f, -0.75f, 3.5f, 0.20f);
        Physics::Entity floor_3(*physics_world, b2_staticBody, Body_Shape::Triangle, 3.35f, -0.5f, 1, 1);

        // --------- Piso Superior ----------------

        // Est�ticos (Derecha a izquierda)
        Physics::Entity floor_4(*physics_world, b2_staticBody, Body_Shape::Polygon, 9.38f, 4.35f, 0.85f, 0.20f);
        Physics::Entity floor_5(*physics_world, b2_staticBody, Body_Shape::Polygon, 7.65f, 3.5f, 1.4f, 0.15f);
        Physics::Entity floor_6(*physics_world, b2_staticBody, Body_Shape::Polygon, 6.20f, 2.5f, 0.55f, 0.20f);


        // Construimos los cuerpos

        player.build_body();

        platform_1.build_body();
        platform_2.build_body();

        floor_1.build_body();
        floor_2.build_body();
        floor_3.build_body();
        floor_4.build_body();
        floor_5.build_body();
        floor_6.build_body();
        
        // Roto uno de los cuerpos 45 grados
        floor_5.get_body()->SetTransform(floor_5.get_body()->GetPosition(), 95.f);
   
    }

    void Scene::update(b2World& _physics_world, float _delta_time)
    {
        physics_world->Step(delta_time, 8, 4);
    }

    void Scene::render(b2World& physics_world, RenderWindow& window, float scale)
    {
        // Se cachea el alto de la ventana en una variable local:

        float window_height = (float)window.getSize().y;

        // Se recorre toda la lista de bodies de physics_world:

        for (b2Body* body = physics_world.GetBodyList(); body != nullptr; body = body->GetNext())
        {
            // Se obtiene el transform del body:

            const b2Transform& body_transform = body->GetTransform();

            // Se recorre la lista de fixtures del body:

            for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext())
            {
                // Se obtiene el tipo de forma de la fixture:

                b2Shape::Type shape_type = fixture->GetShape()->GetType();

                if (shape_type == b2Shape::e_circle)
                {
                    // Se crea un CircleShape a partir de los atributos de la forma de la fixture y del body:
                    // En SFML el centro de un c�rculo no est� en su position. Su position es la esquina superior izquierda
                    // del cuadrado en el que est� inscrito. Por eso a position se le resta el radio tanto en X como en Y.

                    b2CircleShape* circle = dynamic_cast<b2CircleShape*>(fixture->GetShape());

                    float  radius = circle->m_radius * scale;
                    b2Vec2 center = circle->m_p;

                    CircleShape shape;

                    // Enlaza la posici�n del objeto creado en SFML (renderizado) y el objeto de f�sicas creado en Box2D (valores)
                    shape.setPosition(box2d_position_to_sfml_position(b2Mul(body_transform, center), window_height, scale) - Vector2f(radius, radius));
                    shape.setFillColor(Color::Blue);
                    shape.setRadius(radius);

                    window.draw(shape);
                }
                else
                    if (shape_type == b2Shape::e_polygon)
                    {
                        // Se toma la forma poligonal de Box2D (siempre es convexa) y se crea a partir de sus v�rtices un
                        // ConvexShape de SFML. Cada v�rtice de Box2D hay que transformarlo usando el transform del body.

                        b2PolygonShape* box2d_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
                        ConvexShape       sfml_polygon;

                        int number_of_vertices = box2d_polygon->m_count;

                        sfml_polygon.setPointCount(number_of_vertices);
                        sfml_polygon.setFillColor(Color::Blue);

                        for (int index = 0; index < number_of_vertices; index++)
                        {
                            sfml_polygon.setPoint
                            (
                                index,
                                box2d_position_to_sfml_position(b2Mul(body_transform, box2d_polygon->m_vertices[index]), window_height, scale)
                            );
                        }

                        window.draw(sfml_polygon);
                    }
                    else
                        if (shape_type == b2Shape::e_chain)
                        {
                            // Se toma la forma poligonal de Box2D (siempre es convexa) y se crea a partir de sus v�rtices un
                            // ConvexShape de SFML. Cada v�rtice de Box2D hay que transformarlo usando el transform del body.

                            b2ChainShape* box2d_polygon = dynamic_cast<b2ChainShape*>(fixture->GetShape());
                            ConvexShape       sfml_polygon;

                            int number_of_vertices = box2d_polygon->m_count;

                            sfml_polygon.setPointCount(number_of_vertices);
                            sfml_polygon.setFillColor(Color::Blue);

                            for (int index = 0; index < number_of_vertices; index++)
                            {
                                sfml_polygon.setPoint
                                (
                                    index,
                                    box2d_position_to_sfml_position(b2Mul(body_transform, box2d_polygon->m_vertices[index]), window_height, scale)
                                );
                            }

                            window.draw(sfml_polygon);
                        }
            }
        }
    }
};

