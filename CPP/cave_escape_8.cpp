#include <splashkit.h>

const float GRAVITY = 0.08f;
const int MAX_SPEED = 5;
const int JUMP_RECOVERY_BOOST = 2;
const int FOREGROUND_SCROLL_SPEED = -2;
const int BACKGROUND_SCROLL_SPEED = -1;
const int NUM_POLES = 4;

struct pole_data
{
    bool score_limiter;
    sprite up_pole;
    sprite down_pole;
};

typedef struct pole_data poles[NUM_POLES];

struct scene_data
{
    sprite background;
    sprite foreground;
    sprite foreroof;
};

typedef struct game_data
{
    scene_data scene;
    pole_data poles[NUM_POLES];
    sprite player;
} game_data;

sprite get_new_player()
{
    sprite result = create_sprite(bitmap_named("Player"), animation_script_named("PlayerAnimations"));

    sprite_set_x(result, (screen_width() / 2) - sprite_width(result));
    sprite_set_y(result, screen_height() / 2);
    sprite_start_animation(result, "Fly");

    return result;
}

pole_data get_random_poles()
{
    pole_data result;

    result.up_pole = create_sprite(bitmap_named("UpPole"));
    result.down_pole = create_sprite(bitmap_named("DownPole"));

    sprite_set_x(result.up_pole, screen_width() + rnd(1200));
    sprite_set_y(result.up_pole, screen_height() - sprite_height(result.up_pole));

    sprite_set_x(result.down_pole, sprite_x(result.up_pole));
    sprite_set_y(result.down_pole, 0);

    sprite_set_dx(result.up_pole, FOREGROUND_SCROLL_SPEED);
    sprite_set_dx(result.down_pole, FOREGROUND_SCROLL_SPEED);

    return result;
}

scene_data get_new_scene()
{
    scene_data result;

    result.background = create_sprite(bitmap_named("Background"));

    sprite_set_x(result.background, 0);
    sprite_set_y(result.background, 0);
    sprite_set_dx(result.background, BACKGROUND_SCROLL_SPEED);

    result.foreground = create_sprite(bitmap_named("Foreground"), animation_script_named("ForegroundAnimations"));

    sprite_set_x(result.foreground, 0);
    sprite_set_y(result.foreground, screen_height() - sprite_height(result.foreground));
    sprite_set_dx(result.foreground, FOREGROUND_SCROLL_SPEED);
    sprite_start_animation(result.foreground, "Fire");

    result.foreroof = create_sprite(bitmap_named("Foreroof"));

    sprite_set_x(result.foreroof, 0);
    sprite_set_y(result.foreroof, 0);
    sprite_set_dx(result.foreroof, FOREGROUND_SCROLL_SPEED);

    return result;
}

void handle_input(sprite player)
{
    if (key_typed(SPACE_KEY))
    {
        sprite_set_dy(player, sprite_dy(player) - JUMP_RECOVERY_BOOST);
    }
}

void reset_pole_data(pole_data &poles)
{
    free_sprite(poles.up_pole);
    free_sprite(poles.down_pole);

    poles = get_random_poles();
}

void update_velocity(sprite player)
{
    sprite_set_dy(player, sprite_dy(player) + GRAVITY);

    if (sprite_dy(player) > MAX_SPEED)
    {
        sprite_set_dy(player, MAX_SPEED);
    }
    else if (sprite_dy(player) < -MAX_SPEED)
    {
        sprite_set_dy(player, -MAX_SPEED);
    }
}

void update_poles(pole_data &poles)
{
    update_sprite(poles.up_pole);
    update_sprite(poles.down_pole);

    if ((sprite_x(poles.up_pole) + sprite_width(poles.up_pole)) < 0)
    {
        reset_pole_data(poles);
    }
}

void update_pole_array(pole_data pole_array[])
{
    for (int i = 0; i < NUM_POLES; ++i)
    {
        update_poles(pole_array[i]);
    }
}

void update_scene(scene_data &scene)
{
    update_sprite(scene.background);
    update_sprite(scene.foreground);
    update_sprite(scene.foreroof);

    if (sprite_x(scene.background) <= -(sprite_width(scene.background) / 2))
    {
        sprite_set_x(scene.background, 0);
    }

    if (sprite_x(scene.foreground) <= -(sprite_width(scene.foreground) / 2))
    {
        sprite_set_x(scene.foreground, 0);
        sprite_set_x(scene.foreroof, 0);
    }
}

void update_player(sprite &player)
{
    update_velocity(player);
    update_sprite(player);
}

void update_game(game_data &game)
{
    handle_input(game.player);
    update_scene(game.scene);
    update_pole_array(game.poles);
    update_player(game.player);
}

void draw_poles(pole_data &poles)
{
    draw_sprite(poles.up_pole);
    draw_sprite(poles.down_pole);
}

void draw_pole_array(pole_data pole_array[])
{
    for (int i = 0; i < NUM_POLES; ++i)
    {
        draw_poles(pole_array[i]);
    }
}

void draw_game(game_data &game)
{
    draw_sprite(game.scene.background);
    draw_pole_array(game.poles);
    draw_sprite(game.scene.foreroof);
    draw_sprite(game.scene.foreground);
    draw_sprite(game.player);
}

game_data set_up_game()
{
    game_data result;

    load_resource_bundle("CaveEscape", "cave_escape.txt");

    result.scene = get_new_scene();

    for (int i = 0; i < NUM_POLES; ++i)
    {
        result.poles[i] = get_random_poles();
    }

    result.player = get_new_player();

    return result;
}

int main()
{
    // Initialisation
    open_window("Cave Escape", 432, 768);

    game_data game = set_up_game();

    do
    {
        // Update
        process_events();
        update_game(game);

        // Pre-Draw
        clear_screen(COLOR_WHITE);

        // Draw
        draw_game(game);

        // Post-Draw
        refresh_screen();
    } while (!window_close_requested("Cave Escape"));

    // Cleanup
    free_resource_bundle("CaveEscape");
    close_window("Cave Escape");

    return 0;
}
