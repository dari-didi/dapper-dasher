#include "raylib.h"

// custom data structure for scarfy and nebula
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

// ground check
bool isOnGround(AnimData data, int windowHeight){
    return data.pos.y >= windowHeight - data.rec.height;
}

// update animation frame by maxframe in sprite sheet
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame){
    data.runningTime += deltaTime;                                      // delta time since the last frame 
    if(data.runningTime >= data.updateTime){
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;                       // update animation frame
        data.frame ++;
        if (data.frame > maxFrame) {
            data.frame = 0;
        }        
    }
    return data;
}

int main() {
    //array for window dimensions
    int windowDimensions[2];
    windowDimensions[0] = 1000;
    windowDimensions[1] = 720;
 
    // initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dasher Window");
 
    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");    
    AnimData scarfyData{
        {0.0, 0.0, scarfy.width/6, scarfy.height},     // rectangle
        {0, windowDimensions[1] - scarfy.height },     // position to move
        0,                                             // frames to update
        1.0/12.0,                                      // update time
        0.0                                            // running time
    }; 
    
    // nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    int sizeOfNebulae = 10;
    AnimData nebulae[sizeOfNebulae];
    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDimensions[0] + i * 300;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 1.0/(12.0 * i);
        nebulae[i].updateTime = 0;
    }
 
    // scarfy velocity
    int velocity{0};

    // jump velocity (pixels/s)
    int jumpVel{-600};

    // acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};  

    // nebula X velocity (pixels/s) 
    int nebVel{-200};

    // set last nebula as finish line    
    float finishLine{ nebulae[sizeOfNebulae-1].pos.x };

    // initialize collision bool
    bool collision{false};

    // background image variables, background, foreground and midground
    float bgX{};
    float mgX{};
    float fgX{};
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
   
    SetTargetFPS(60);
    while(!WindowShouldClose()){                                    // true when pressed close
        // delta time (time passed since the last frame)
        const float dT{GetFrameTime()};

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // scroll background
        bgX -= 100 * dT;                                            // speed of background scroll
        if(bgX <= -background.width * 4){
           bgX = 0.0; 
        }
        // scroll midground
        mgX -= 200 * dT;
        if(mgX <= -midground.width * 4){
            mgX = 0.0;
        }
        // scroll foreground
        fgX -= 300 * dT;
        if(fgX <= -foreground.width * 4){
            fgX = 0.0;
        }

        // draw background, midground, foreground
        Vector2 bg1Pos{bgX, 0.0};
        Vector2 mg1Pos{mgX, 0.0};
        Vector2 fg1Pos{fgX, 0.0};
        Vector2 bg2Pos{bgX + background.width * 4, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 4, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 4, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 4.0, WHITE);

        DrawTextureEx(midground, mg1Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 4.0, WHITE);

        DrawTextureEx(foreground, fg1Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 4.0, WHITE);

        // groundcheck, set no gravity when on the ground
        if(isOnGround(scarfyData, windowDimensions[1])){
            velocity = 0;                                               //set no gravity
        } else {
            velocity += gravity * dT;                                   //scarfy is in the air, set gravity
        }

        // jump check space key and prevent air jumping 
        if(IsKeyPressed(KEY_SPACE) && isOnGround(scarfyData, windowDimensions[1]) ){
        //if(IsKeyPressed(KEY_SPACE)){                                  //exclude airjumping check 
            velocity += jumpVel;                                        //set the jumping speed with jumpVel
        }
 
        // update scarfy position
        scarfyData.pos.y += velocity * dT;       
        
        for ( int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebVel *dT;                             // update nebula position set nebula speed with nebVel
        }

        //update finish line
        finishLine += nebVel *dT; 

        //collision check of scarfy and nebula
        for (AnimData nebula : nebulae)
        {
            float pad{50};                                              // create pad for empty space in sprite sheet
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }          
        }

        if(collision){
            // game over if collision
            DrawText("Game Over", windowDimensions[0]/4, windowDimensions[1]/2, 100, RED);
        } else if (scarfyData.pos.x >= finishLine){
            // win game if scarfy reaches finishline
            DrawText("You WIN!", windowDimensions[0]/4, windowDimensions[1]/2, 100, WHITE);
        } else { 
            // keep drawing textures when game is not ended
            
            // draw scarfy and update animation frame
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            scarfyData = updateAnimData(scarfyData, dT, 5);

            // draw nebula and update animation frame
            for ( int i = 0; i < sizeOfNebulae; i++)
            {
                nebulae[i] = updateAnimData(nebulae[i], dT, 7);
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

        }
        EndDrawing();
    }
    //shut down properly unload loaded textures
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}