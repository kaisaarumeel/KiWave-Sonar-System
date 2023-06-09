<script>
    import { Layer } from "svelte-canvas";
    import { getCoordinates } from "./utils";
    import { sonarCommands, sonarStore, darkModeSwitch } from "../../../data/stores";

    export let screenRadius;

    $: dist1 = parseFloat($sonarStore.sonarData.rRange1);
    $: dist2 = parseFloat($sonarStore.sonarData.rRange2);
    $: deg1 = parseInt($sonarStore.sonarData.rDeg1);
    $: deg2 = parseInt($sonarStore.sonarData.rDeg2);
    $: range = parseInt($sonarCommands.sonarData.sRange);
    let history = [];

    const activeSectorColor = "#007AFF"
    
    // Reactively change colors based on darkmode switch value
    $: objectColor = $darkModeSwitch.isDark ? "#c3c1c1" : "#767676";
    $: lineToObjectColor = $darkModeSwitch.isDark ? "#3b3b3b" : "#d5d7d4";
        
    $: objectColorAdj = $darkModeSwitch.isDark ? "rgba(213, 215, 212, *OPC*)" : "rgba(118, 118, 118, *OPC*)";
    $: lineToObjectColorAdj = $darkModeSwitch.isDark ? "rgba(59, 59, 59, *OPC*)" : "rgba(213, 215, 212, *OPC*)";
    /**
     * Draws 1 line from the center to the specified coordinates to erase 
     * previous object measurments. If called on the currently measured (active) sector, the line corresponding to the 
     * degree will be highlighted.
     * @param context The graphical context
     * @param deg The angle at which the object was detected
     * @param dist The distance from the sensor
     * @param width Canvas width
     * @param height Canvas height 
     * @param currentlyActive Boolean flag indicating whether the function is called on the currently measured (active) sector
     * @param color Color of the line drawn
     */
    const drawLineTo = (context, deg, dist, width, height, currentlyActive, color) => {
            const endCoordinates = getCoordinates(deg, dist, $sonarCommands.sonarData.sRange, screenRadius);
            context.save();
            context.translate(width / 2, height / 2);
            context.beginPath();
            context.strokeStyle = color;
            context.lineWidth = 5;
            context.moveTo(0, 0);
            context.lineTo(endCoordinates.x, endCoordinates.y);
            context.stroke();
            context.restore();
    
        // Draws the highlighted line last so it's not drawn on and covered
        if (currentlyActive) {
            const endCoordinates = getCoordinates(deg, dist, $sonarCommands.sonarData.sRange, screenRadius);
            context.save();
            context.translate(width / 2, height / 2);
            context.beginPath();
            context.strokeStyle = activeSectorColor;
            context.lineWidth = 1;
            context.moveTo(0, 0);
            context.lineTo(endCoordinates.x, endCoordinates.y);
            context.stroke();
            context.restore();
        }
    };


    /**
     * Draws 1 line from the specified coordinates to the edge of the 
     * radar screen, which represents a detected object. If called on the currently measured (active) sector, the line corresponding to the 
     * degree will be highlighted.
     * @param context The graphcial context
     * @param deg The angle at which the object was detected
     * @param dist The distance from the sensor 
     * @param width Canvas width
     * @param height Canvas height 
     * @param currentlyActive Boolean flag indicating whether the function is called on the currently measured (active) sector
     * @param color Color of the line drawn    
     */
    const drawObjectFrom = (context, deg, dist, width, height, currentlyActive, color) => {
        // Calculate coordinates for the endpoint for the line on the circle
            const startCoordinates = getCoordinates(deg, dist, $sonarCommands.sonarData.sRange, screenRadius);
            const endCoordinates = getCoordinates(deg, $sonarCommands.sonarData.sRange, $sonarCommands.sonarData.sRange, screenRadius);
            context.save();
            context.translate(width / 2, height / 2);
            context.beginPath();
            context.strokeStyle = color;
            context.lineWidth = 5;
            context.moveTo(startCoordinates.x, startCoordinates.y);
            context.lineTo(endCoordinates.x, endCoordinates.y);
            context.stroke();
            context.restore();
        
        // Draws the highlighted line last so it's not drawn on and covered
        if (currentlyActive) {
            const startCoordinates = getCoordinates(deg, dist, $sonarCommands.sonarData.sRange, screenRadius);
            const endCoordinates = getCoordinates(deg, $sonarCommands.sonarData.sRange, $sonarCommands.sonarData.sRange, screenRadius);
            context.save();
            context.translate(width / 2, height / 2);
            context.beginPath();
            context.strokeStyle = activeSectorColor;
            context.lineWidth = 1;
            context.moveTo(startCoordinates.x, startCoordinates.y);
            context.lineTo(endCoordinates.x, endCoordinates.y);
            context.stroke();
            context.restore();
        }
    };

    /**
     * Renders the Object component to the Canvas element. The Object component will contain the two sectors the sensor is currently 
     * looking at (indicated by a highlighted line in the middle), as well any previously measured objects, if there are any.
     * By declaring this function reactively, we make sure svelte-canvas re-render anytime the values the function depends on change. 
     * @param context The 2D rendering context of the Canvas element
     * @param width Canvas width
     * @param height Canvas height 
     */
    $: render = ({ context, width, height }) => {
        for (let i = 0; i < history.length; i++) {
            let opacity=((1.0/history.length)*i).toString();

            if (history[i].dist1 > range) history[i].dist1 = range;
            drawObjectFrom(context, history[i].deg1, history[i].dist1, width, height, false,objectColorAdj.replace("*OPC*",opacity));
            drawLineTo(context, history[i].deg1, history[i].dist1, width, height, false,lineToObjectColor.replace("*OPC*",opacity));

            if (history[i].dist2 > range) history[i].dist2 = range;
            drawObjectFrom(context, history[i].deg2, history[i].dist2, width, height, false,objectColorAdj.replace("*OPC*",opacity));
            drawLineTo(context, history[i].deg2, history[i].dist2, width, height, false,lineToObjectColor.replace("*OPC*",opacity));
        }

        if (dist1 > range) dist1 = range;
        drawObjectFrom(context, deg1, dist1, width, height, true,objectColor);
        drawLineTo(context, deg1, dist1, width, height, true,lineToObjectColor);

        if (dist2 > range) dist2 = range;
        drawObjectFrom(context, deg2, dist2, width, height, true,objectColor);
        drawLineTo(context, deg2, dist2, width, height, true,lineToObjectColor);
        if(history.length>360){
            //If the history contains more than 360 elements we remove the first one, to prevent performance issues.
            history.shift();
        } else {
            setTimeout(async function(){
                history.shift();

            },10000)
        }
        history.push({
            "deg1": deg1,
            "deg2": deg2,
            "dist1": dist1,
            "dist2": dist2
        })
        
    }


</script>

<Layer {render} />