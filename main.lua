print("Testing raylib bindings...")
assert(raylib, "raylib table not found")
assert(raylib.DrawCircle, "DrawCircle not found")
assert(raylib.DrawRectangle, "DrawRectangle not found")
assert(raylib.DrawText, "DrawText not found")
assert(raylib.DrawLine, "DrawLine not found")
print("All raylib functions found")

-- Store previous positions for trail effect
local trailPoints = {}
local maxTrailLength = 50

function onRender(x, y, frame)
    -- Main center circle
    local radius = 50
    local circleColor = {
        r = math.abs(math.sin(frame * 0.02)) * 255,
        g = math.abs(math.sin(frame * 0.03)) * 255,
        b = math.abs(math.sin(frame * 0.01)) * 255,
        a = 255
    }
    
    raylib.DrawCircle(x, y, radius, circleColor)
    
    -- Create multiple orbiting circles
    local numOrbiters = 5
    for i = 1, numOrbiters do
        local orbitRadius = 80 + i * 30
        local orbitSpeed = 0.3 / i
        local phase = (2 * math.pi * i) / numOrbiters
        
        local orbitX = x + math.cos(frame * orbitSpeed + phase) * orbitRadius
        local orbitY = y + math.sin(frame * orbitSpeed + phase) * orbitRadius
        
        -- Store trail points
        if not trailPoints[i] then
            trailPoints[i] = {}
        end
        
        table.insert(trailPoints[i], {x = orbitX, y = orbitY})
        if #trailPoints[i] > maxTrailLength then
            table.remove(trailPoints[i], 1)
        end
        
        -- Draw trails
        for j = 2, #trailPoints[i] do
            local alpha = (j / #trailPoints[i]) * 255
            local trailColor = {
                r = (i * 50) % 255,
                g = (i * 70) % 255,
                b = (i * 90) % 255,
                a = alpha
            }
            local prev = trailPoints[i][j-1]
            local curr = trailPoints[i][j]
            raylib.DrawLine(prev.x, prev.y, curr.x, curr.y, trailColor)
        end
        
        -- Draw orbiting circle
        local orbitingCircleRadius = 15
        local orbitingCircleColor = {
            r = (i * 50) % 255,
            g = (i * 70) % 255,
            b = (i * 90) % 255,
            a = 255
        }
        raylib.DrawCircle(orbitX, orbitY, orbitingCircleRadius, orbitingCircleColor)
    end
    
    -- Draw title with pulsing effect
    local textSize = 20 + math.sin(frame * 0.1) * 5
    raylib.DrawText("Orbital Light Show", 
        x - 100, 
        50, 
        textSize, 
        {r = 255, g = 255, b = 255, a = 255})
end
