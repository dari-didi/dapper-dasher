#include "raylib.h"


struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight){
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame){
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime){
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame ++;
        if (data.frame > maxFrame) {
            data.frame = 0;
        }        
    }
    return data;
}

int main()
{

    int windowDimensions[2];
    windowDimensions[0] = 1280;
    windowDimensions[1] = 720;
    InitWindow(windowDimensions[0], windowDimensions[1], "Dasher Window");


    // nebula variables
 
    Texture2D scarfy = LoadTexture("textures/scarfy.png");    
    AnimData scarfyData{
        {0.0, 0.0, scarfy.width/6, scarfy.height},     // rectangle
        {0, windowDimensions[1] - scarfy.height },     // position to move
        0,                                             // frames to update
        1.0/12.0,                                      // update time
        0.0                                            // running time
    }; 
    
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
    
    float finishLine{ nebulae[sizeOfNebulae-1].pos.x };
    
    bool isInAir{false};

    // nebula X velocity (pixels/second)
    int nebVel{-200};
    //rectangle dimensions
    int velocity{0};
    // jump velocity (pixels/sec)
    int jumpVel{-15000};
    const int gravity{15'500};  

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    float mgX{};
    float fgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        const float dT{GetFrameTime()};

        BeginDrawing();
        ClearBackground(WHITE);
        // draw the background

        bgX -= 200 * dT;
        if(bgX <= -background.width * 4 ){
           bgX = 0.0; 
        }

        mgX -= 300 * dT;
        if(mgX <= -midground.width * 4){
            mgX = 0.0;
        }
        fgX -= 400 * dT;
        if(fgX <= -foreground.width * 4){
            fgX = 0.0;
        }
        Vector2 bg1Pos{bgX, 0.0};
        Vector2 mg1Pos{mgX, 0.0};
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(midground, mg1Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(foreground, fg1Pos, 0.0, 4.0, WHITE);

        Vector2 bg2Pos{bgX + background.width * 4, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 4, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 4, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 4.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 4.0, WHITE);
        //delta time (time since last frame)

        if(isOnGround(scarfyData, windowDimensions[1])){
            velocity = 0;
            isInAir = false;            
        } else {
            velocity =+ gravity * dT;
            isInAir = true;
        }

        if(IsKeyPressed(KEY_SPACE) && isOnGround ){
            velocity += jumpVel;
        }
        // update scarfy position
        scarfyData.pos.y += velocity * dT;
        
        scarfyData = updateAnimData(scarfyData, dT, 5);

        // update nebula`s animation frame
        for ( int i = 0; i < sizeof(nebulae)/sizeof(nebulae[0]); i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            nebulae[i].pos.x += nebVel *dT;                    // update nebula position
        }
        
        //update finish line
        finishLine += nebVel *dT; 

        // draw scarfy
        DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

        EndDrawing();
    }
    //shut down properly
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}
