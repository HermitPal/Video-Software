-- Function to animate a circle from left to right
function animate_circle(x_start, x_end, duration, fps)
    local frames = duration * fps
    for i = 0, frames do
        BeginTextureMode(target)  -- Start drawing to texture
        ClearBackground(BLACK)    -- Clear with black background
        local x = x_start + (x_end - x_start) * (i / frames)
        drawCircle(x, 540, 50)   -- Draw circle
        EndTextureMode()         -- End drawing to texture
        captureFrame()           -- Capture and send frame
    end
end

-- Start the animation
animate_circle(100, 1700, 30, 30)  -- 30 second animation
print("Animation complete!")