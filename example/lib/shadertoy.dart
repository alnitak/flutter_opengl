/// Shaders taken from ShaderToy.com
/// These are only fragment shaders
/// Many of the shaders can be copy/pasted.
/// Also many of them are heavy for mobile devices (few FPS)
/// "iMouse", "iTime", "iResolution", "iChannel0" and "iChannel1" uniforms
/// are currently supported

List<Map<String, String>> shaderToy = [
  {
    'url': 'TEXTURE TEST',
    'fragment':
'''
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 iR = iResolution.xy;
    vec2 uv = fragCoord/iR;

    float n = .5;
    if (uv.x >= n-0.01 && uv.x <= n+0.01)
        fragColor = vec4(1., 1., 0., 1.0);

    else
    if (uv.y >= n-0.01 && uv.y <= n+0.01)
        fragColor = vec4(1., 1., 0., 1.0);

    else
    if (uv.x < n) {
        fragColor = texture(iChannel0, uv);
    } else {
       fragColor = texture(iChannel1, uv);
      // fragColor = vec4(0.,0.,0.,0.);
    }
}
'''
  },

  { // https://www.shadertoy.com/view/XsjXW1
    'url': 'https://www.shadertoy.com/view/XsjXW1',
    'fragment':
    '''
    mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

// the backbone of any noise, or really any chaotic/natural/random looking
// shader. hash basically maps an input value n to a random location in [0..1]
// this function does have a period of TWO_PI but because the scaling factor
// is so large there's significant differences between e.g. hash(0) and hash(TWO_PI)
// the float data-type is not precise enough to be able to show the pattern even
// in minute scales
// and we're sampling with n ranging in the hundreds and thousands - it's basically
// a reproducable random
float hash( float n )
{
    return fract(sin(n)*43758.5453123);
}


// some random noise function that i can't really parse
// most noise functions are basically chaotic but
// still continuous mappings whose behavior is complex enough
// that humans can't see the pattern
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}




// return an interpolated height and position for the given input

// height algorithm:
// partition 2d space into a set of 1x1 squares
// each square has 4 corners; each corner's coordinate is associated
// with a random height (and with a variable a-d), such that the same corner on two different squares has
// the same value. Interpolate between the corners
// depending on where in the 1x1 square you are. Linear interpolation
// creates discontinuities at the edges, instead use a cubic hermite spline with slope 0 at the tangents - this ensures smoothness
// of 2 degrees.
/*
      x+           
 +------------>   
 |                
 |  a            b
y|   +----------+ 
+|   |          | 
 |   |          | 
 v   |          | 
     |          | 
     |          | 
     +----------+ 
    c            d
*/
vec3 noised( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);

    vec2 u = f*f*(3.0-2.0*f);

    float n = p.x + p.y*54.0;

    // heights of the four corners
    float a = hash(n+  0.0);
    float b = hash(n+  1.0);
    float c = hash(n+ 54.0);
    float d = hash(n+ 55.0);
    
    return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,
                // scalar function: f*f*(f*(f-2.0)+1.0) = f^2*(f-1)^2 = hump shape from [0..1] (*30 = goes up to 1.875 in the middle)
                // any function can serve as the scalar as long as it 1) goes to 0 at the edges, and 2) is continuous (and preferably is smooth as well)
                // the scalar affects how smooth the terrain is; a higher value will make the mountains more sharp
                
                // the actual yz vec2 is another returned value that to add more flavor to the noise. 
                // You can use it as the basis of another noise, or (in the case of the caller here), 
                // modify the .x with the .yz.
                // the vec2 function should reference a,b,c,d such that the function evaluates to the
                // same value on either side of the edge of the 1x1 square
                
                /* 
				 * if the formula is F(x), the following invariants should hold:
				 * (1) F(<x+, y>).y = F(<x-, y>).y, x is an integer
				 * (2) F(<x, y+>).x = F(<x, y->).x, y is an integer
				 * in (1), the scalar makes the x component go to 0, so only the y components must
				 * be equal. Similarly in (2), the y component is multiplied by 0, so only the x
				 * components should be equal.
				 * With the invariants, transitioning between two cells of the 1x1 grid will be continuous
				 */
                30.0*f*f*(f*(f-2.0)+1.0)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));

}

float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0;

    float res = mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                    mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);

    return res;
}

float fbm( vec3 p )
{
    float f = 0.0;

    f += 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p );

    return f/0.9375;
}

mat2 m2 = mat2(1.6,-1.2,1.2,1.6);

// fbm is a more complex noise, again harmonic
// that takes several layers of noise at differing
// scales and adds them up -- basically a more interesting
// noise
float fbm( vec2 p )
{
    float f = 0.0;

    f += 0.5000*noise( p ); p = m2*p*2.02;
    f += 0.2500*noise( p ); p = m2*p*2.03;
    f += 0.1250*noise( p ); p = m2*p*2.01;
    f += 0.0625*noise( p );

    return f/0.9375;
}

// the height of the terrain at the given xy location
float terrain( in vec2 x )
{
    // scale world coordinates down a lot
    vec2  p = x*0.003;
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);
    // sample a base noise at different locations 5 times
    // and accumulate the sample into the final height
    // this is the technique of taking a simple noise
    // and making more interesting noise out of it by
    // doing harmonic sampling? is that what it's called
    for(int i=0;i<5; i++)
    {
        // random n
        vec3 n = noised(p);
        // d represents some "distance" away from some origin
        d += n.yz;
        // b is the octave attenuation (1, 0.5, 0.25, etc)
        // add this octave, which is scaled by the attenuation
        // factor and also by the distance factor
        a += b*n.x/(1.0+dot(d,d));
        b *= 0.5;
        // not sure what how this matrix acts on p
        // i believe it's more or less a twist + expand
        p=m2*p;
    }

    return 140.0*a;
}

// same as terrain but go for 9 more iterations
// this produces a much more fine-grained terrain function
// that has goes deeper into the smaller scales of the terrain
// so if terrain( x ) returned some value like 100,
// terrain2( x ) returns a slightly more detailed value like 100.125
// or something - it's a purturbed version of normal terrain
// but the purtubations also exhibit the fractal landscape behavior
// that is characterstic of the land
float terrain2( in vec2 x )
{
    vec2  p = x*0.003;
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);
    for(int i=0;i<14; i++)
    {
        vec3 n = noised(p);
        d += n.yz;
        a += b*n.x/(1.0+dot(d,d));
        b *= 0.5;
        p=m2*p;
    }

    return 140.0*a;
}


// takes in a world location and returns how vertically
// far away you are from the terrain there
// when used within raymarching this means that as the ray 
// is marching, since the map is continuous, you'll eventually
// get within some small epsilon value of the actual terrain,
// at which point you can count it as a hit
float map( in vec3 p )
{
    // the height of the terrain at the location's xz plane
    float h = terrain(p.xz);

    // some sort of postprocessing of the height here
    // to give it more jagged edges

    // not sure how it really works yet
    float ss = 0.03;
    float hh = h*ss;
    // scale the height down a lot, decompose it into its fractional
    // and whole parts
    float fh = fract(hh);
    float ih = floor(hh);
    // transform fh by pushing it up towards its sqrt
    // by some amount depending on the height of the terrain you're at
    // fh corresponds to a height of ~33 in the terrain
    // the effect of this is that regularly at 33 pixel intervals,
    // the middle area of the interval gets pushed up towards the top
    // this creates more of a plateau effect which is more realistic
    // of actual terrain
    fh = mix( sqrt(fh), fh, smoothstep(50.0,140.0,h) );
    
    // reconstruct h with the new fractional part
    h = (ih+fh)/ss;

    return p.y - h;
}

// same as map but using terrain2
float map2( in vec3 p )
{
    float h = terrain2(p.xz);


    float ss = 0.03;
    float hh = h*ss;
    float fh = fract(hh);
    float ih = floor(hh);
    fh = mix( sqrt(fh), fh, smoothstep(50.0,140.0,h) );
    h = (ih+fh)/ss;

    return p.y - h;
}

// return whether the ray starting at rO and pointing at rD
// hit the map or not. If it did, the distance from the origin to the hit
// will be stored in resT
// assumes rD is normalized
bool jinteresct(in vec3 rO, in vec3 rD, out float resT )
{
    float h = 0.0;
    float t = 0.0;
    // march the ray forward 120 times
    for( int j=0; j<120; j++ )
    {
        //if( t>2000.0 ) break;

        vec3 p = rO + t*rD;
        // break out of the iteration if you've passed some vertical
        // plane which guarantees you won't hit the map
        // (assumes the map is below 300); it's for performance i think?
if( p.y>300.0 ) break;
        h = map( p );

        // if you're within .1 units, count that as a hit on the terrain
        if( h<0.1 )
        {
            resT = t;
            return true;
        }
        // t controls where your ray has been marched to
        // what iq is saying here is that if the height is
        // still large (that is, the ray is still quite vertically
        // away from hitting the terrain), you can jump the
        // ray much farther forward
        // the invariant here is that the terrain's slope is less than 2
        // the invariant is broken sometimes which is where the strange
        // aliased banding that moves with the camera comes from
        // this is really cool -- you're dynamically sampling the terrain
        // based on how far away you are from it
        t += max(0.1,0.5*h);
    }

    // if you've reached here, you've gone 120 iterations and still
    // not hit the terrain, OR you've hit y = 300
    // if you're within 5 units of the floor, just say you hit
    // not sure why he does this
    if( h<5.0 )
    {
        resT = t;
        return true;
    }
    return false;
}

// this is called starting at the original ray's intersection point + 20*light1, in the direction of light1
// it calculates a long-range shadow with a soft-edge
float sinteresct(in vec3 rO, in vec3 rD )
{
    float res = 1.0;
    float t = 0.0;
    for( int j=0; j<50; j++ )
    {
        //if( t>1000.0 ) break;
        // cast a ray towards the light
        vec3 p = rO + t*rD;

        // track how far from the ground you are
        float h = map( p );

        // if you've hit the ground, you're completely in the shadow
        if( h<0.1 )
        {
            return 0.0;
        }
        // 16 * h/t?
        // 16 is an arbitrary scalar
        // h/t is the difference in height divided by the distance the ray has travelled
        res = min( res, 16.0*h/t );
        // adding h is another performance gain - move the ray based on how far you away you are now
        // because the terrain is mostly smooth
        // you can basically analyze this as t will at some point get placed very near the actual minimum distance
        // away between the ray and the terrain
        // at that point, res will be set to h/t -- so farther-away terrains will cast shadows with a larger gradient
        // basically emulates the diffusive quality of light, also realy cool
        t += h;

    }

    return clamp( res, 0.0, 1.0 );
}

// approximate the gradient of the terrain at pos by
// moving in a small epsilon in each of the three directions
// the epsilon is scaled by t, the distance of the point from
// the camera. This means that points farther away
// have a smoother normal calculation
// the choice of epsilon has a massive effect on the percieved
// detail of the terrain
// the difference is clearly noticable if you have a small epsilon
// on faraway points - there is a lot of aliasing since the effective
// distance between two adjacent pixels that are both far away
// is quite large. This achieves the same effect as mip-mapping in terms
// of detail. You do in fact want to scale by `t` otherwise pixels far away
// don't look connected
vec3 calcNormal( in vec3 pos, float t )
{
    float e = 0.001 * t;
    vec3  eps = vec3(e,0.0,0.0);
    vec3 nor;
    nor.x = map2(pos+eps.xyy) - map2(pos-eps.xyy);
    nor.y = map2(pos+eps.yxy) - map2(pos-eps.yxy);
    nor.z = map2(pos+eps.yyx) - map2(pos-eps.yyx);
    return normalize(nor);
}

vec3 camPath( float time )
{
    vec2 p = 600.0*vec2( cos(1.4+0.37*time),
                         cos(3.2+0.31*time) );

    return vec3( p.x, 0.0, p.y );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // maps screen from (-1, -1) on bottom-left to (1, 1) on top-right
    vec2 xy = -1.0 + 2.0*fragCoord.xy / iResolution.xy;

    // correct aspect ratio; y goes from -1 to 1, x goes from -1.75 to 1.75 (the aspect ratio is hard-coded to be 1.75)
    vec2 s = xy*vec2(1.75,1.0);

    float time = iTime*0.15;

    // diretional light coming down from the top
    vec3 light1 = normalize( vec3(  0.4, 0.22,  0.6 ) );

    // a second light, probably directional
    vec3 light2 = vec3( -0.707, 0.000, -0.707 );

    // camera position is set using two cosines at different speeds
    vec3 campos = camPath( time );
    // target's always looking forward -- this is cool
    vec3 camtar = camPath( time + 3.0 );
    campos.y = terrain( campos.xz ) + 15.0;
    // the eye is always just looking down
    camtar.y = campos.y*0.5;

    // roll the camera
    float roll = 0.1*cos(0.1*time);
    // cp is an upwards pointing vector in world space that wiggles slightly
    // it controls the up vector of the camera
    vec3 cp = vec3(sin(roll), cos(roll),0.0);

    // the direction the camera's pointing at
    vec3 cw = normalize(camtar-campos);
    // points to the "right" in the camera
    vec3 cu = normalize(cross(cw,cp));
    // points "up" in the camera, basically cp projected onto the camera's
    // viewing plane
    vec3 cv = normalize(cross(cu,cw));
    // direction of the ray to cast for this pixel
    // it's made of 3 components in camera space -
    // an amount to go horizontally,
    // an amount to go vertically,
    // and an amount to go outwards
    // that cw vector is needed otherwise the rays are always
    // perpendicular to the camera's viewing angle
    // the cw factor more or less controls the fov
    // (a large factor zooms the whole frustum in, a small factor
    // expands the fov out)
    // this is shoddily recreating a frustum, i don't
    // believe this is the right way to do this
    vec3 rd = normalize( s.x*cu + s.y*cv + 1.6*cw );

    float sundot = clamp(dot(rd,light1),0.0,1.0);
    vec3 col;
    float t;
    // if your ray doesn't hit the ground, this pixel is hitting
    // the sky instead -- render the sky
    if( !jinteresct(campos,rd,t) )
    {
        // sky is very light blue scaled by the ray's y position,
        // which is how high up the ray is; higher up means higher up
        // in the sky means a darker blue;
        // the y never goes past some small threshold
        // so the sky always looks *mostly* blue, but you can still see
        // the gradient
        col = 0.9*vec3(0.97,.99,1.0)*(1.0-0.3*rd.y);

        // this line adds a small amount of red-orangish color
        // mostly in a ring shape around where the "sun" would be
        col += 0.2*vec3(0.8,0.7,0.5)*pow( sundot, 4.0 );
    }
    else
    {
        // here's where you hit
        vec3 pos = campos + t*rd;

        // the terrain normal at this location
        vec3 nor = calcNormal( pos, t );

        //basic diffuse light calculation
        float dif1 = clamp( dot( light1, nor ), 0.0, 1.0 );
        // i think this basically produces some ambient light as well?
        float dif2 = clamp( 0.2 + 0.8*dot( light2, nor ), 0.0, 1.0 );
        
        // shadow factor -- 1 = no long-range shadow, 0 = completely in shadow
        float sh = 1.0;
        // if light1 hits me
        if( dif1>0.001 )
            sh = sinteresct(pos+light1*20.0,light1);

        // dif1v is the color contribution of the diffuse of the first light source
        vec3 dif1v = vec3(dif1);
        // sh = iMouse.x / iResolution.x;
        // soft-shadow has a red coloring to it
        dif1v *= vec3( sh, sh*sh*0.5+0.5*sh, sh*sh );

        // apply a simple random noise factor on the base ground color
        // just to give it a bit more flavor
        float r = noise( 7.0*pos.xz );

        // here we begin coloring the terrain. The base color mixes between
        // a dark brown base color and a slightly more colorful brown at the top
        // terrain2 is used here as just another random noise function whose input scales
        // are on the same magnitude as the position; it's used to give the vertical bands
        // in the terrain
        float heightMix = clamp(terrain2( vec2(pos.x,pos.y*48.0))/200.0,0.0,1.0);
        col = (r*0.25+0.75)*0.9*mix( vec3(0.10,0.05,0.03), vec3(0.13,0.10,0.08), heightMix );
        // mix the color with a more reddish hue if the normal points more up (how flat it is)
        col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*r),smoothstep(0.70,0.9,nor.y) );
        // and if they're *really* flat, give it some green for grass
        col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*r),smoothstep(0.95,1.0,nor.y) );
        col *= 0.75;
         // snow
        #if 1
        // height factor -- higher places have more snow (h=1), lower places have no snow (h=0)
        float h = smoothstep(55.0,80.0,pos.y + 25.0*fbm(0.01*pos.xz) );
        // normal+cliff factor -- land that is flatter gets more snow, cliff-like land doesn't get much snow
        // normal+cliff factor is also scaled by height, aka higher up the slope factor is stronger
        float e = smoothstep(1.0-0.5*h,1.0-0.1*h,nor.y);
        // wind directional factor -- if you have a high x slope, put less snow.
        // so hilly terrain that is going up in one direction will have snow,
        // going down on the other side will have no snow
        float o = 0.3 + 0.7*smoothstep(0.0,0.1,nor.x+h*h);
        float s = h*e*o;
        s = smoothstep( 0.1, 0.9, s );
        // mix color with dark grey color for snow
        col = mix( col, 0.4*vec3(0.6,0.65,0.7), s );
        #endif


        // final light calculation
        
        // start with a slope factor -- cliffs will be shaded slightly darker
        // this is most apparent when the cliff meets the ground; emulates ambient occlusion
        // a little bit
        vec3 brdf  = 2.0*vec3(0.17,0.19,0.20)*clamp(nor.y,0.0,1.0);
        // most of the lighting comes from dif1
             brdf += 6.0*vec3(1.00,0.95,0.80)*dif1v;
        // a bit from dif2, which is just a small ambient secondary light to complement the first
             brdf += 2.0*vec3(0.20,0.20,0.20)*dif2;

       	// add all of the light
        col *= brdf;

        // fog factor - 0 at close distances, goes to 1. Has a slightly s shaped curve
        float fo = 1.0-exp(-pow(0.0015*t,1.5));
        // fog color -- base 0.7; if you're looking towards the sun, make the fog the color of the sun
        // when there's a lot of fog in the distance, the fog carries the color of the sun and blends
        // it in with the actual surface color. This emulates that behavior
        vec3 fco = vec3(0.7) + 0.6*vec3(0.8,0.7,0.5)*pow( sundot, 4.0 );
        col = mix( col, fco, fo );
    }

    // postprocess filter; basically a sqrt color curve that helps the dark parts look brighter
    // a bit like a compressor audio effect; makes things a bit warmer in general
    col = sqrt(col);

    // (0, 0) to (1, 1)
    vec2 uv = xy*0.5+0.5;
    //create a slight inset shadow on the top corners
    col *= 0.7 + 0.3*pow(16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y),0.1);

    // and we're done!
    fragColor=vec4(col,1.0);
}
    '''
  },
  { // https://www.shadertoy.com/view/XttSRs
    'url': 'https://www.shadertoy.com/view/XttSRs',
    'fragment':
    '''
// Constants
#define PI 3.1415
#define MOD2 vec2(3.07965, 7.4235)

// Raymarching
float minPrimStepSize = 0.4 ;
const int primNumSamples = 150 ;

// Colours
const vec3 obsidianCol = vec3(30./255., 31./255., 30./255.) ;
const vec3 sunCol = vec3(1.0,1.0,1.0) ;
const vec3 skyCol = vec3(206./255., 146./255., 14./255.) ;
const float ambientCol = 0.3 ;

// Sun
const float lightElev = 20. * 3.14/180. ;
const float lightAzi = 100. * 3.14/180. ;
const vec3 lightDir = vec3(cos(lightAzi)*cos(lightElev),sin(lightElev),sin(lightAzi)*cos(lightElev));

// Terrain
const int octavesTerrain =  12 ;
const float sharpness = 0.35 ;
const float offset = 0.85 ;

// Lava

vec2 rot2D(vec2 p, float angle) {
    angle = radians(angle);
    float s = sin(angle);
    float c = cos(angle);
    
    return p * mat2(c,s,-s,c);  
}

float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float hash12(vec2 p) {
	p  = fract(p / MOD2);
    p += dot(p.xy, p.yx+19.19);
    return fract(p.x * p.y);
}

const vec2 add = vec2(1.0, 0.0);

float noise( in vec2 x ) {
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f*f*(3.0-2.0*f);
    float res = mix(mix( hash12(p),          hash12(p + add.xy),f.x),
                    mix( hash12(p + add.yx), hash12(p + add.xx),f.x),f.y);
    return res;
}

float noiseEmber(vec2 p) {
    return hash12(p);
}

float smoothNoiseEmber(vec2 p) {
    vec2 p0 = floor(p + vec2(0.0, 0.0));
    vec2 p1 = floor(p + vec2(1.0, 0.0));
    vec2 p2 = floor(p + vec2(0.0, 1.0));
    vec2 p3 = floor(p + vec2(1.0, 1.0));
    vec2 pf = fract(p);
    return mix( mix(noise(p0), noise(p1), pf.x),mix(noise(p2), noiseEmber(p3), pf.x), pf.y);
}

const int octavesEmber = 5 ;

float fbmEmber(in vec2 p) {
    float f=0.0;
    for(int i=0; i < octavesEmber; ++i)
        f+=smoothNoiseEmber(2.0*p*exp2(float(i)))/exp2(float(i+1));
    return f;
}

float sampleField(vec3 position) {
  float noiseVal = 0.0 ;
  float amplitude = 1.0 ;
  float scaling = 75. ;
  float freq = 0.009 ;
  float lac = 2.0 ;
  for (int i = 0 ; i < octavesTerrain ; ++i) {
    noiseVal += amplitude * noise(freq*position.xz) ;
    amplitude /= lac ;
    freq *= lac ;
  }
    
  // Islandise terrain //
  noiseVal -= offset ;
  noiseVal = clamp(noiseVal,0.,1.5) ;
  noiseVal = pow(sharpness, noiseVal) ;
  noiseVal = 1.0 - (noiseVal) ;
  return noiseVal * scaling ;
}

#define time iTime*0.1

float hash21(in vec2 n){ return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453); }
mat2 makem2(in float theta){float c = cos(theta);float s = sin(theta);return mat2(c,-s,s,c);}
float noiseLava( in vec2 x ){return texture(iChannel0, x*0.02).x;}

float sampleFreq = 0.2 ;
vec2 gradn(vec2 p) {
	float ep = .09;
	float gradx = noiseLava(sampleFreq*vec2(p.x+ep,p.y))-noiseLava(sampleFreq*vec2(p.x-ep,p.y));
	float grady = noiseLava(sampleFreq*vec2(p.x,p.y+ep))-noiseLava(sampleFreq*vec2(p.x,p.y-ep));
	return vec2(gradx,grady);
}


// ...... Taken from https://www.shadertoy.com/view/MdBSRW
#define TEMPERATURE 2200.0

vec3 blackbody(float t)
{
    t *= TEMPERATURE;
    
    float u = ( 0.860117757 + 1.54118254e-4 * t + 1.28641212e-7 * t*t ) 
            / ( 1.0 + 8.42420235e-4 * t + 7.08145163e-7 * t*t );
    
    float v = ( 0.317398726 + 4.22806245e-5 * t + 4.20481691e-8 * t*t ) 
            / ( 1.0 - 2.89741816e-5 * t + 1.61456053e-7 * t*t );

    float x = 3.0*u / (2.0*u - 8.0*v + 4.0);
    float y = 2.0*v / (2.0*u - 8.0*v + 4.0);
    float z = 1.0 - x - y;
    
    float Y = 1.0;
    float X = Y / y * x;
    float Z = Y / y * z;

    mat3 XYZtoRGB = mat3(3.2404542, -1.5371385, -0.4985314,
                        -0.9692660,  1.8760108,  0.0415560,
                         0.0556434, -0.2040259,  1.0572252);

    return max(vec3(0.0), (vec3(X,Y,Z) * XYZtoRGB) * pow(t * 0.0004, 4.0));
}


// ..... Taken from https://www.shadertoy.com/view/lslXRS
float flowFBM(in vec2 p)
{
	float z=2.;
	float rz = 0.;
	vec2 bp = p;
	for (float i= 1.;i < 9.;i++ ){
		//primary flow speed
		p += time*.006;
		//secondary flow speed (speed of the perceived flow)
		bp += time*0.00009;
		//displacement field (try changing time multiplier)
		vec2 gr = gradn(i*p*1.4+time*0.1);
		//rotation of the displacement field
		gr*=makem2(time*2.-(0.05*p.x+0.03*p.y)*40.);
		//displace the system
		p += gr*.5;
		//add noise octave
		rz+= (sin(noise(p)*7.)*0.5+0.5)/z;
		//blend factor (blending displaced system with base system)
		//you could call this advection factor (.5 being low, .95 being high)
		p = mix(bp,p,.77);
		//intensity scaling
		z *= 1.4;
		//octave scaling
		p *= 3.1;
		bp *= 1.5;
	}
	return rz;	
}


vec3 getFieldNormal(in vec3 cameraPos, in vec3 direction, float dist) {
  float p = min(.3, .0005+.00005 * dist*dist);
  vec3 position = cameraPos + direction * dist ;
  vec3 nor = vec3(0.0,sampleField(position), 0.0);
  vec3 v2 = nor-vec3(p,sampleField(position+vec3(p,0.0,0.0)), 0.0);
  vec3 v3 = nor-vec3(0.0,sampleField(position+vec3(0.0,0.0,-p)), -p);
  nor = cross(v2, v3);
  return normalize(nor);   
}

vec3 calcSkyCol(in vec3 direction)
{
	float sunAmount = max( dot(direction, lightDir), 0.0 );
	float v = pow(1.0-max(direction.y,0.0),5.)*.5;
	vec3  sky = vec3(v*sunCol.x*0.4+skyCol.x, v*sunCol.y*0.4+skyCol.y, v*sunCol.z*0.4+skyCol.z);
	sky = sky + sunCol * pow(sunAmount, 6.5)*.32;
	sky = sky+ sunCol * min(pow(sunAmount, 1600.), .3)*.65;
	return sky;
}

float fineRayMarchBinarySearch(vec3 startPos, vec3 direction, in vec2 rayDists, out float height) {
    float halfwayT;
	for (int n = 0; n < 4; n++)
	{
		halfwayT = (rayDists.x + rayDists.y) * .5;
		vec3 p = startPos + halfwayT*direction;
		if ((height = sampleField(p)) - p.y > 0.5) {
			rayDists.x = halfwayT;
		} else {
			rayDists.y = halfwayT;
		}
	}
	return rayDists.x;
}

bool coarseRayMarch(vec3 startPos, vec3 direction, out float rayDist, out float height) {
    vec3 position = startPos ;
    bool intersected = false ;
    rayDist = 0.0 ;
    float oldRayDist = rayDist ;
    
    for (int i = 0 ; i < primNumSamples ; ++i) {
        if (position.y > 66.) {
            break ;
        }
		if ((height = sampleField(position)) - position.y > 0.5) {            
            intersected = true ;
            break ;
        } else {
            oldRayDist = rayDist ;
            float delta = max(minPrimStepSize,height*0.001) + (rayDist*0.025);
		    rayDist += delta;
            position = (rayDist)*direction + startPos ;
        }
    }
    
    if (intersected) {
       rayDist = fineRayMarchBinarySearch(startPos,direction,vec2(rayDist,oldRayDist),height) ;   
    }
    
    return intersected ;
}

// Calculate sun light...
vec3 calcColour(in vec3 pos, in vec3 normal, in vec3 eyeDir, float dis, float height)
{
  	float h = dot(lightDir,normal);
	float c = max(h, 0.0)+ambientCol;
    vec2 samplePosition = (eyeDir * dis + pos).xz  ;
    vec3 lavCol = blackbody(flowFBM(samplePosition*0.005)) ;
    float specular = 0.0 ;
    vec3 col ;
    if (height > 0.0) {
        specular = 0.6 ;
        col = obsidianCol ;
        col = col * (sunCol + lavCol * 6.0/(height)) * c ;
        if (h  > 0.0) {
          vec3 R = reflect(lightDir, normal);
	  	  float specAmount = pow( max(dot(R, normalize(eyeDir)), 0.0), 3.0)*specular;
		  col = mix(col, sunCol , specAmount);
        }
    } else {
        col = lavCol ;
    }
    
    return col ;
}

vec3 calcFog( in vec3  rgb, in float dis, in vec3 dir) {
	float fogAmount = exp(-dis* 0.002);
	return mix(calcSkyCol(dir), rgb, fogAmount);
}
    

void mainImage( out vec4 fragColor, in vec2 fragCoord)
{    
    vec2 uv = (fragCoord.xy - iResolution.xy * .5) / iResolution.y;
    vec2  m = 2.*((iMouse.xy / iResolution.xy) - 0.5);
    
    if (iMouse.xy == vec2(0)) {
       m.y = 0.0 ;   
    }
    
    vec3 dir = vec3(uv, 1.);
    dir.yz = rot2D(dir.yz,  90. * m.y);
    dir.xz = rot2D(dir.xz, 180. * m.x + 180.);
    dir = normalize(dir) ;

    float offset = iTime ;
    vec3 cameraPos = vec3(0.0,0.0,3.0*offset) ;
    float heightOffset = sampleField(cameraPos) ;
    cameraPos.y = 20.0 + heightOffset ;
    float rayInterDist = -1.0 ;
    float height = 0.0 ;
	bool hasIntersected = coarseRayMarch(cameraPos,dir,rayInterDist,height) ;
    vec3 col = vec3(0.0) ;
    if (hasIntersected) {
      vec3 normal = getFieldNormal(cameraPos,dir,rayInterDist) ;
      col = calcColour(cameraPos + dir*rayInterDist, normal, dir, rayInterDist, height) ;
    } else {
      col = calcSkyCol(dir) ;     
    }
    col =  calcFog(col,rayInterDist,dir) ;
    
    // Taken from 
    vec3 embers=vec3(1.0,0.35,0.04)*smoothstep(0.77+sin(time*5.0)*0.01+sin(time)*0.01,1.0,fbmEmber(uv*10.0+vec2(cos(uv.y*0.8+time*1.0)*10.0,time*40.0)));
    embers+=vec3(1.0,0.35,0.04)*smoothstep(0.77+sin(time*82.0)*0.01+sin(time*1.2)*0.01,1.0,fbmEmber(vec2(100.0)+uv*8.0+vec2(time*1.0+cos(uv.y*0.3+time*1.0)*10.0,time*30.0)));

	fragColor = vec4(col + 8.*embers,1) ;
}
'''
  },
  {   // https://www.shadertoy.com/view/ltffzl
    'url': 'https://www.shadertoy.com/view/ltffzl',
    'fragment':
    '''
// Heartfelt - by Martijn Steinrucken aka BigWings - 2017
// Email:countfrolic@gmail.com Twitter:@The_ArtOfCode
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// I revisited the rain effect I did for another shader. This one is better in multiple ways:
// 1. The glass gets foggy.
// 2. Drops cut trails in the fog on the glass.
// 3. The amount of rain is adjustable (with Mouse.y)

// To have full control over the rain, uncomment the HAS_HEART define 

// A video of the effect can be found here:
// https://www.youtube.com/watch?v=uiF5Tlw22PI&feature=youtu.be

// Music - Alone In The Dark - Vadim Kiselev
// https://soundcloud.com/ahmed-gado-1/sad-piano-alone-in-the-dark
// Rain sounds:
// https://soundcloud.com/elirtmusic/sleeping-sound-rain-and-thunder-1-hours

#define S(a, b, t) smoothstep(a, b, t)
//#define CHEAP_NORMALS
#define HAS_HEART
#define USE_POST_PROCESSING

vec3 N13(float p) {
    //  from DAVE HOSKINS
   vec3 p3 = fract(vec3(p) * vec3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

vec4 N14(float t) {
	return fract(sin(t*vec4(123., 1024., 1456., 264.))*vec4(6547., 345., 8799., 1564.));
}
float N(float t) {
    return fract(sin(t*12345.564)*7658.76);
}

float Saw(float b, float t) {
	return S(0., b, t)*S(1., b, t);
}


vec2 DropLayer2(vec2 uv, float t) {
    vec2 UV = uv;
    
    uv.y += t*0.75;
    vec2 a = vec2(6., 1.);
    vec2 grid = a*2.;
    vec2 id = floor(uv*grid);
    
    float colShift = N(id.x); 
    uv.y += colShift;
    
    id = floor(uv*grid);
    vec3 n = N13(id.x*35.2+id.y*2376.1);
    vec2 st = fract(uv*grid)-vec2(.5, 0);
    
    float x = n.x-.5;
    
    float y = UV.y*20.;
    float wiggle = sin(y+sin(y));
    x += wiggle*(.5-abs(x))*(n.z-.5);
    x *= .7;
    float ti = fract(t+n.z);
    y = (Saw(.85, ti)-.5)*.9+.5;
    vec2 p = vec2(x, y);
    
    float d = length((st-p)*a.yx);
    
    float mainDrop = S(.4, .0, d);
    
    float r = sqrt(S(1., y, st.y));
    float cd = abs(st.x-x);
    float trail = S(.23*r, .15*r*r, cd);
    float trailFront = S(-.02, .02, st.y-y);
    trail *= trailFront*r*r;
    
    y = UV.y;
    float trail2 = S(.2*r, .0, cd);
    float droplets = max(0., (sin(y*(1.-y)*120.)-st.y))*trail2*trailFront*n.z;
    y = fract(y*10.)+(st.y-.5);
    float dd = length(st-vec2(x, y));
    droplets = S(.3, 0., dd);
    float m = mainDrop+droplets*r*trailFront;
    
    //m += st.x>a.y*.45 || st.y>a.x*.165 ? 1.2 : 0.;
    return vec2(m, trail);
}

float StaticDrops(vec2 uv, float t) {
	uv *= 40.;
    
    vec2 id = floor(uv);
    uv = fract(uv)-.5;
    vec3 n = N13(id.x*107.45+id.y*3543.654);
    vec2 p = (n.xy-.5)*.7;
    float d = length(uv-p);
    
    float fade = Saw(.025, fract(t+n.z));
    float c = S(.3, 0., d)*fract(n.z*10.)*fade;
    return c;
}

vec2 Drops(vec2 uv, float t, float l0, float l1, float l2) {
    float s = StaticDrops(uv, t)*l0; 
    vec2 m1 = DropLayer2(uv, t)*l1;
    vec2 m2 = DropLayer2(uv*1.85, t)*l2;
    
    float c = s+m1.x+m2.x;
    c = S(.3, 1., c);
    
    return vec2(c, max(m1.y*l0, m2.y*l1));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (fragCoord.xy-.5*iResolution.xy) / iResolution.y;
    vec2 UV = fragCoord.xy/iResolution.xy;
    vec3 M = iMouse.xyz/iResolution.xyz;
    float T = iTime+M.x*2.;
    
    #ifdef HAS_HEART
    T = mod(iTime, 102.);
    T = mix(T, M.x*102., M.z>0.?1.:0.);
    #endif
    
    
    float t = T*.2;
    
    float rainAmount = iMouse.z>0. ? M.y : sin(T*.05)*.3+.7;
    
    float maxBlur = mix(3., 6., rainAmount);
    float minBlur = 2.;
    
    float story = 0.;
    float heart = 0.;
    
    #ifdef HAS_HEART
    story = S(0., 70., T);
    
    t = min(1., T/70.);						// remap drop time so it goes slower when it freezes
    t = 1.-t;
    t = (1.-t*t)*70.;
    
    float zoom= mix(.3, 1.2, story);		// slowly zoom out
    uv *=zoom;
    minBlur = 4.+S(.5, 1., story)*3.;		// more opaque glass towards the end
    maxBlur = 6.+S(.5, 1., story)*1.5;
    
    vec2 hv = uv-vec2(.0, -.1);				// build heart
    hv.x *= .5;
    float s = S(110., 70., T);				// heart gets smaller and fades towards the end
    hv.y-=sqrt(abs(hv.x))*.5*s;
    heart = length(hv);
    heart = S(.4*s, .2*s, heart)*s;
    rainAmount = heart;						// the rain is where the heart is
    
    maxBlur-=heart;							// inside the heart slighly less foggy
    uv *= 1.5;								// zoom out a bit more
    t *= .25;
    #else
    float zoom = -cos(T*.2);
    uv *= .7+zoom*.3;
    #endif
    UV = (UV-.5)*(.9+zoom*.1)+.5;
    
    float staticDrops = S(-.5, 1., rainAmount)*2.;
    float layer1 = S(.25, .75, rainAmount);
    float layer2 = S(.0, .5, rainAmount);
    
    
    vec2 c = Drops(uv, t, staticDrops, layer1, layer2);
   #ifdef CHEAP_NORMALS
    	vec2 n = vec2(dFdx(c.x), dFdy(c.x));// cheap normals (3x cheaper, but 2 times shittier ;))
    #else
    	vec2 e = vec2(.001, 0.);
    	float cx = Drops(uv+e, t, staticDrops, layer1, layer2).x;
    	float cy = Drops(uv+e.yx, t, staticDrops, layer1, layer2).x;
    	vec2 n = vec2(cx-c.x, cy-c.x);		// expensive normals
    #endif
    
    
    #ifdef HAS_HEART
    n *= 1.-S(60., 85., T);
    c.y *= 1.-S(80., 100., T)*.8;
    #endif
    
    float focus = mix(maxBlur-c.y, minBlur, S(.1, .2, c.x));
    vec3 col = textureLod(iChannel0, UV+n, focus).rgb;
    
    
    #ifdef USE_POST_PROCESSING
    t = (T+3.)*.5;										// make time sync with first lightnoing
    float colFade = sin(t*.2)*.5+.5+story;
    col *= mix(vec3(1.), vec3(.8, .9, 1.3), colFade);	// subtle color shift
    float fade = S(0., 10., T);							// fade in at the start
    float lightning = sin(t*sin(t*10.));				// lighting flicker
    lightning *= pow(max(0., sin(t+sin(t))), 10.);		// lightning flash
    col *= 1.+lightning*fade*mix(1., .1, story*story);	// composite lightning
    col *= 1.-dot(UV-=.5, UV);							// vignette
    											
    #ifdef HAS_HEART
    	col = mix(pow(col, vec3(1.2)), col, heart);
    	fade *= S(102., 97., T);
    #endif
    
    col *= fade;										// composite start and end fade
    #endif
    
    //col = vec3(heart);
    fragColor = vec4(col, 1.);
}
'''
  },
  {
    // https://www.shadertoy.com/view/ls3cDB
    'url': 'https://www.shadertoy.com/view/ls3cDB',
    'fragment':
    '''
#define pi 3.14159265359
#define radius .1

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float aspect = iResolution.x / iResolution.y;

    vec2 uv = fragCoord * vec2(aspect, 1.) / iResolution.xy;
    vec2 mouse = iMouse.xy  * vec2(aspect, 1.) / iResolution.xy;
    vec2 mouseDir = normalize(abs(iMouse.zw) - iMouse.xy);
    vec2 origin = clamp(mouse - mouseDir * mouse.x / mouseDir.x, 0., 1.);
    
    float mouseDist = clamp(length(mouse - origin) 
        + (aspect - (abs(iMouse.z) / iResolution.x) * aspect) / mouseDir.x, 0., aspect / mouseDir.x);
    
    if (mouseDir.x < 0.)
    {
        mouseDist = distance(mouse, origin);
    }
  
    float proj = dot(uv - origin, mouseDir);
    float dist = proj - mouseDist;
    
    vec2 linePoint = uv - dist * mouseDir;
    
    if (dist > radius) 
    {
        fragColor = texture(iChannel1, uv * vec2(1. / aspect, 1.));
        fragColor.rgb *= pow(clamp(dist - radius, 0., 1.) * 1.5, .2);
    }
    else if (dist >= 0.)
    {
        // map to cylinder point
        float theta = asin(dist / radius);
        vec2 p2 = linePoint + mouseDir * (pi - theta) * radius;
        vec2 p1 = linePoint + mouseDir * theta * radius;
        uv = (p2.x <= aspect && p2.y <= 1. && p2.x > 0. && p2.y > 0.) ? p2 : p1;
        fragColor = texture(iChannel0, uv * vec2(1. / aspect, 1.));
        fragColor.rgb *= pow(clamp((radius - dist) / radius, 0., 1.), .2);
    }
    else 
    {
        vec2 p = linePoint + mouseDir * (abs(dist) + pi * radius);
        uv = (p.x <= aspect && p.y <= 1. && p.x > 0. && p.y > 0.) ? p : uv;
        fragColor = texture(iChannel0, uv * vec2(1. / aspect, 1.));
    }
}
'''
  },
  {
    // https://www.shadertoy.com/view/dlfXRn
    'url': 'https://www.shadertoy.com/view/dlfXRn',
    'fragment':
    '''
float warp = 0.75; // simulate curvature of CRT monitor
float zoom_factor;
float abberationoffset = 0.1;

void mainImage(out vec4 O, vec2 funni)
{
    warp = sin(iTime);
    //comment out the line above if you dont want the screen to zoom in and out
    zoom_factor =  1. + ((warp / 15.) * -1.);
    vec2 p = funni.xy / iResolution.xy;
    vec4 toUse = vec4(texture(iChannel0,p));
    // squared distance from center
    vec2 uv = p;
    vec2 redoffsetfunniredamgus = uv;
    vec2 blueoffsetlikedeeznuts = uv;
    vec2 dc = abs(0.5-uv);
    dc *= dc;
    
    // warp the fragment coordinates
    uv.x -= 0.5; uv.x *= 1.0+(dc.y*(0.3*warp)); uv.x += 0.5;
    uv.y -= 0.5; uv.y *= 1.0+(dc.x*(0.2*warp)); uv.y += 0.5;

    // warp the red/blue offset coordinates
    redoffsetfunniredamgus.x -= 0.5; redoffsetfunniredamgus.x *= 1.0+(dc.y*((0.3 + abberationoffset)*warp)); redoffsetfunniredamgus.x += 0.5;
    redoffsetfunniredamgus.y -= 0.5; redoffsetfunniredamgus.y *= 1.0+(dc.x*((0.2 + abberationoffset)*warp)); redoffsetfunniredamgus.y += 0.5;
    blueoffsetlikedeeznuts.x -= 0.5; blueoffsetlikedeeznuts.x *= 1.0+(dc.y*((0.3 - abberationoffset)*warp)); blueoffsetlikedeeznuts.x += 0.5;
    blueoffsetlikedeeznuts.y -= 0.5; blueoffsetlikedeeznuts.y *= 1.0+(dc.x*((0.2 - abberationoffset)*warp)); blueoffsetlikedeeznuts.y += 0.5;

    // zoom in the uv because uh yeah
    uv = uv + (uv - vec2(0.5, 0.5)) * (zoom_factor - 1.0);
    redoffsetfunniredamgus = redoffsetfunniredamgus + (redoffsetfunniredamgus - vec2(0.5, 0.5)) * (zoom_factor - 1.0);
    blueoffsetlikedeeznuts = blueoffsetlikedeeznuts + (blueoffsetlikedeeznuts - vec2(0.5, 0.5)) * (zoom_factor - 1.0);

    // sample inside boundaries, otherwise set to black
    if (uv.y > 1.0 || uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0)
        O = vec4(0.0,0.0,0.0,1.0);
    else
    {
        // sample the texture
        vec4 col1 = vec4(texture(iChannel0,uv));
        vec4 col2 = vec4(texture(iChannel0,redoffsetfunniredamgus));
		//vec4 col2 = texture2D(bitmap, redoffsetfunniredamgus);
        vec4 col3 = vec4(texture(iChannel0,blueoffsetlikedeeznuts));
		//vec4 col3 = texture2D(bitmap, blueoffsetlikedeeznuts);
    	O = vec4(col2.r, col1.g, col3.b, col1.a);
    }
}
'''
  },
  {
    // https://www.shadertoy.com/view/XdXGR7
    'url': 'https://www.shadertoy.com/view/XdXGR7',
    'fragment':
    '''
const float PI = 3.14159265359;
#define time (-iTime*5.0)

const vec3 eps = vec3(0.01, 0.0, 0.0);

float genWave1(float len)
{
	float wave = sin(8.0 * PI * len + time);
	wave = (wave + 1.0) * 0.5; // <0 ; 1>
	wave -= 0.3;
	wave *= wave * wave;
	return wave;
}

float genWave2(float len)
{
	float wave = sin(7.0 * PI * len + time);
	float wavePow = 1.0 - pow(abs(wave*1.1), 0.8);
	wave = wavePow * wave; 
	return wave;
}

float scene(float len)
{
	// you can select type of waves
	return genWave1(len);
}

vec2 normal(float len) 
{
	float tg = (scene(len + eps.x) - scene(len)) / eps.x;
	return normalize(vec2(-tg, 1.0));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	vec2 so = iMouse.xy / iResolution.xy;
	vec2 pos2 = vec2(uv - so); 	  //wave origin
	vec2 pos2n = normalize(pos2);

	float len = length(pos2);
	float wave = scene(len); 

	vec2 uv2 = -pos2n * wave/(1.0 + 5.0 * len);

	fragColor = vec4(texture(iChannel0, uv + uv2));
}
'''
  },
  {
    // https://www.shadertoy.com/view/XlfGRj
    'url': 'https://www.shadertoy.com/view/XlfGRj',
    'fragment': '''
/*--------------------------------------------------------------------------------------
License CC0 - http://creativecommons.org/publicdomain/zero/1.0/
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
----------------------------------------------------------------------------------------
^ This means do ANYTHING YOU WANT with this code. Because we are programmers, not lawyers.
-Otavio Good
*/

// ---------------- Config ----------------
// This is an option that lets you render high quality frames for screenshots. It enables
// stochastic antialiasing and motion blur automatically for any shader.
//#define NON_REALTIME_HQ_RENDER
const float frameToRenderHQ = 50.0; // Time in seconds of frame to render
const float antialiasingSamples = 16.0; // 16x antialiasing - too much might make the shader compiler angry.

//#define MANUAL_CAMERA


// --------------------------------------------------------
// These variables are for the non-realtime block renderer.
float localTime = 0.0;
float seed = 1.0;

// Animation variables
float fade = 1.0;
vec3 sunDir;
vec3 sunCol;
float exposure = 1.0;
vec3 skyCol, horizonCol;

// other
float marchCount = 0.0;

// ---- noise functions ----
float v31(vec3 a)
{
    return a.x + a.y * 37.0 + a.z * 521.0;
}
float v21(vec2 a)
{
    return a.x + a.y * 37.0;
}
float Hash11(float a)
{
    return fract(sin(a)*10403.9);
}
float Hash21(vec2 uv)
{
    float f = uv.x + uv.y * 37.0;
    return fract(sin(f)*104003.9);
}
vec2 Hash22(vec2 uv)
{
    float f = uv.x + uv.y * 37.0;
    return fract(cos(f)*vec2(10003.579, 37049.7));
}
vec2 Hash12(float f)
{
    return fract(cos(f)*vec2(10003.579, 37049.7));
}
float Hash1d(float u)
{
    return fract(sin(u)*143.9);	// scale this down to kill the jitters
}
float Hash2d(vec2 uv)
{
    float f = uv.x + uv.y * 37.0;
    return fract(sin(f)*104003.9);
}
float Hash3d(vec3 uv)
{
    float f = uv.x + uv.y * 37.0 + uv.z * 521.0;
    return fract(sin(f)*110003.9);
}
float mixP(float f0, float f1, float a)
{
    return mix(f0, f1, a*a*(3.0-2.0*a));
}
const vec2 zeroOne = vec2(0.0, 1.0);
float noise2d(vec2 uv)
{
    vec2 fr = fract(uv.xy);
    vec2 fl = floor(uv.xy);
    float h00 = Hash2d(fl);
    float h10 = Hash2d(fl + zeroOne.yx);
    float h01 = Hash2d(fl + zeroOne);
    float h11 = Hash2d(fl + zeroOne.yy);
    return mixP(mixP(h00, h10, fr.x), mixP(h01, h11, fr.x), fr.y);
}
float noise(vec3 uv)
{
    vec3 fr = fract(uv.xyz);
    vec3 fl = floor(uv.xyz);
    float h000 = Hash3d(fl);
    float h100 = Hash3d(fl + zeroOne.yxx);
    float h010 = Hash3d(fl + zeroOne.xyx);
    float h110 = Hash3d(fl + zeroOne.yyx);
    float h001 = Hash3d(fl + zeroOne.xxy);
    float h101 = Hash3d(fl + zeroOne.yxy);
    float h011 = Hash3d(fl + zeroOne.xyy);
    float h111 = Hash3d(fl + zeroOne.yyy);
    return mixP(
        mixP(mixP(h000, h100, fr.x),
             mixP(h010, h110, fr.x), fr.y),
        mixP(mixP(h001, h101, fr.x),
             mixP(h011, h111, fr.x), fr.y)
        , fr.z);
}

const float PI=3.14159265;

vec3 saturate(vec3 a) { return clamp(a, 0.0, 1.0); }
vec2 saturate(vec2 a) { return clamp(a, 0.0, 1.0); }
float saturate(float a) { return clamp(a, 0.0, 1.0); }

// This function basically is a procedural environment map that makes the sun
vec3 GetSunColorSmall(vec3 rayDir, vec3 sunDir)
{
	vec3 localRay = normalize(rayDir);
	float dist = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
	float sunIntensity = 0.05 / dist;
    sunIntensity += exp(-dist*150.0)*7000.0;
	sunIntensity = min(sunIntensity, 40000.0);
	return sunCol * sunIntensity*0.025;
}

vec3 GetEnvMap(vec3 rayDir, vec3 sunDir)
{
    // fade the sky color, multiply sunset dimming
    vec3 finalColor = mix(horizonCol, skyCol, pow(saturate(rayDir.y), 0.47))*0.95;
    // make clouds - just a horizontal plane with noise
    float n = noise2d(rayDir.xz/rayDir.y*1.0);
    n += noise2d(rayDir.xz/rayDir.y*2.0)*0.5;
    n += noise2d(rayDir.xz/rayDir.y*4.0)*0.25;
    n += noise2d(rayDir.xz/rayDir.y*8.0)*0.125;
    n = pow(abs(n), 3.0);
    n = mix(n * 0.2, n, saturate(abs(rayDir.y * 8.0)));  // fade clouds in distance
    finalColor = mix(finalColor, (vec3(1.0)+sunCol*10.0)*0.75*saturate((rayDir.y+0.2)*5.0), saturate(n*0.125));

    // add the sun
    finalColor += GetSunColorSmall(rayDir, sunDir);
    return finalColor;
}

vec3 GetEnvMapSkyline(vec3 rayDir, vec3 sunDir, float height)
{
    vec3 finalColor = GetEnvMap(rayDir, sunDir);

    // Make a skyscraper skyline reflection.
    float radial = atan(rayDir.z, rayDir.x)*4.0;
    float skyline = floor((sin(5.3456*radial) + sin(1.234*radial)+ sin(2.177*radial))*0.6);
    radial *= 4.0;
    skyline += floor((sin(5.0*radial) + sin(1.234*radial)+ sin(2.177*radial))*0.6)*0.1;
    float mask = saturate((rayDir.y*8.0 - skyline-2.5+height)*24.0);
    float vert = sign(sin(radial*32.0))*0.5+0.5;
    float hor = sign(sin(rayDir.y*256.0))*0.5+0.5;
    mask = saturate(mask + (1.0-hor*vert)*0.05);
    finalColor = mix(finalColor * vec3(0.1,0.07,0.05), finalColor, mask);

	return finalColor;
}

// min function that supports materials in the y component
vec2 matmin(vec2 a, vec2 b)
{
    if (a.x < b.x) return a;
    else return b;
}

// ---- shapes defined by distance fields ----
// See this site for a reference to more distance functions...
// https://iquilezles.org/articles/distfunctions

// signed box distance field
float sdBox(vec3 p, vec3 radius)
{
  vec3 dist = abs(p) - radius;
  return min(max(dist.x, max(dist.y, dist.z)), 0.0) + length(max(dist, 0.0));
}

// capped cylinder distance field
float cylCap(vec3 p, float r, float lenRad)
{
    float a = length(p.xy) - r;
    a = max(a, abs(p.z) - lenRad);
    return a;
}

// k should be negative. -4.0 works nicely.
// smooth blending function
float smin(float a, float b, float k)
{
	return log2(exp2(k*a)+exp2(k*b))/k;
}

float Repeat(float a, float len)
{
    return mod(a, len) - 0.5 * len;
}

// Distance function that defines the car.
// Basically it's 2 boxes smooth-blended together and a mirrored cylinder for the wheels.
vec2 Car(vec3 baseCenter, float unique)
{
    // bottom box
    float car = sdBox(baseCenter + vec3(0.0, -0.008, 0.001), vec3(0.01, 0.00225, 0.0275));
    // top box smooth blended
    car = smin(car, sdBox(baseCenter + vec3(0.0, -0.016, 0.008), vec3(0.005, 0.0005, 0.01)), -160.0);
    // mirror the z axis to duplicate the cylinders for wheels
    vec3 wMirror = baseCenter + vec3(0.0, -0.005, 0.0);
    wMirror.z = abs(wMirror.z)-0.02;
    float wheels = cylCap((wMirror).zyx, 0.004, 0.0135);
    // Set materials
    vec2 distAndMat = vec2(wheels, 3.0);	// car wheels
    // Car material is some big number that's unique to each car
    // so I can have each car be a different color
    distAndMat = matmin(distAndMat, vec2(car, 100000.0 + unique));	// car
    return distAndMat;
}

// How much space between voxel borders and geometry for voxel ray march optimization
float voxelPad = 0.2;
// p should be in [0..1] range on xz plane
// pint is an integer pair saying which city block you are on
vec2 CityBlock(vec3 p, vec2 pint)
{
    // Get random numbers for this block by hashing the city block variable
    vec4 rand;
    rand.xy = Hash22(pint);
    rand.zw = Hash22(rand.xy);
    vec2 rand2 = Hash22(rand.zw);

    // Radius of the building
    float baseRad = 0.2 + (rand.x) * 0.1;
    baseRad = floor(baseRad * 20.0+0.5)/20.0;	// try to snap this for window texture

    // make position relative to the middle of the block
    vec3 baseCenter = p - vec3(0.5, 0.0, 0.5);
    float height = rand.w*rand.z + 0.1; // height of first building block
    // Make the city skyline higher in the middle of the city.
    float downtown = saturate(4.0 / length(pint.xy));
    height *= downtown;
    height *= 1.5+(baseRad-0.15)*20.0;
    height += 0.1;	// minimum building height
    //height += sin(iTime + pint.x);	// animate the building heights if you're feeling silly
    height = floor(height*20.0)*0.05;	// height is in floor units - each floor is 0.05 high.
	float d = sdBox(baseCenter, vec3(baseRad, height, baseRad)); // large building piece

    // road
    d = min(d, p.y);

    //if (length(pint.xy) > 8.0) return vec2(d, mat);	// Hack to LOD in the distance

    // height of second building section
    float height2 = max(0.0, rand.y * 2.0 - 1.0) * downtown;
    height2 = floor(height2*20.0)*0.05;	// floor units
    rand2 = floor(rand2*20.0)*0.05;	// floor units
    // size pieces of building
	d = min(d, sdBox(baseCenter - vec3(0.0, height, 0.0), vec3(baseRad, height2 - rand2.y, baseRad*0.4)));
	d = min(d, sdBox(baseCenter - vec3(0.0, height, 0.0), vec3(baseRad*0.4, height2 - rand2.x, baseRad)));
    // second building section
    if (rand2.y > 0.25)
    {
		d = min(d, sdBox(baseCenter - vec3(0.0, height, 0.0), vec3(baseRad*0.8, height2, baseRad*0.8)));
        // subtract off piece from top so it looks like there's a wall around the roof.
        float topWidth = baseRad;
        if (height2 > 0.0) topWidth = baseRad * 0.8;
		d = max(d, -sdBox(baseCenter - vec3(0.0, height+height2, 0.0), vec3(topWidth-0.0125, 0.015, topWidth-0.0125)));
    }
    else
    {
        // Cylinder top section of building
		if (height2 > 0.0) d = min(d, cylCap((baseCenter - vec3(0.0, height, 0.0)).xzy, baseRad*0.8, height2));
    }
    // mini elevator shaft boxes on top of building
	d = min(d, sdBox(baseCenter - vec3((rand.x-0.5)*baseRad, height+height2, (rand.y-0.5)*baseRad),
                     vec3(baseRad*0.3*rand.z, 0.1*rand2.y, baseRad*0.3*rand2.x+0.025)));
    // mirror another box (and scale it) so we get 2 boxes for the price of 1.
    vec3 boxPos = baseCenter - vec3((rand2.x-0.5)*baseRad, height+height2, (rand2.y-0.5)*baseRad);
    float big = sign(boxPos.x);
    boxPos.x = abs(boxPos.x)-0.02 - baseRad*0.3*rand.w;
	d = min(d, sdBox(boxPos,
    vec3(baseRad*0.3*rand.w, 0.07*rand.y, baseRad*0.2*rand.x + big*0.025)));

    // Put domes on some building tops for variety
    if (rand.y < 0.04)
    {
        d = min(d, length(baseCenter - vec3(0.0, height, 0.0)) - baseRad*0.8);
    }

    //d = max(d, p.y);  // flatten the city for debugging cars

    // Need to make a material variable.
    vec2 distAndMat = vec2(d, 0.0);
    // sidewalk box with material
    distAndMat = matmin(distAndMat, vec2(sdBox(baseCenter, vec3(0.35, 0.005, 0.35)), 1.0));

    return distAndMat;
}

// This is the distance function that defines all the scene's geometry.
// The input is a position in space.
// The output is the distance to the nearest surface and a material index.
vec2 DistanceToObject(vec3 p)
{
    //p.y += noise2d((p.xz)*0.0625)*8.0; // Hills
    vec3 rep = p;
    rep.xz = fract(p.xz); // [0..1] for representing the position in the city block
    vec2 distAndMat = CityBlock(rep, floor(p.xz));

    // Set up the cars. This is doing a lot of mirroring and repeating because I
    // only want to do a single call to the car distance function for all the
    // cars in the scene. And there's a lot of traffic!
    vec3 p2 = p;
    rep.xyz = p2;
    float carTime = localTime*0.2;  // Speed of car driving
    float crossStreet = 1.0;  // whether we are north/south or east/west
    float repeatDist = 0.25;  // Car density bumper to bumper
    // If we are going north/south instead of east/west (?) make cars that are
    // stopped in the street so we don't have collisions.
    if (abs(fract(rep.x)-0.5) < 0.35)
    {
        p2.x += 0.05;
        p2.xz = p2.zx * vec2(-1.0,1.0);  // Rotate 90 degrees
        rep.xz = p2.xz;
        crossStreet = 0.0;
        repeatDist = 0.1;  // Denser traffic on cross streets
    }
    
    rep.z += floor(p2.x);	// shift so less repitition between parallel blocks
    rep.x = Repeat(p2.x - 0.5, 1.0);	// repeat every block
    rep.z = rep.z*sign(rep.x);	// mirror but keep cars facing the right way
    rep.x = (rep.x*sign(rep.x))-0.09;
    rep.z -= carTime * crossStreet;	// make cars move
    float uniqueID = floor(rep.z/repeatDist);	// each car gets a unique ID that we can use for colors
    rep.z = Repeat(rep.z, repeatDist);	// repeat the line of cars every quarter block
    rep.x += (Hash11(uniqueID)*0.075-0.01);	// nudge cars left and right to take both lanes
    float frontBack = Hash11(uniqueID*0.987)*0.18-0.09;
    frontBack *= sin(localTime*2.0 + uniqueID);
    rep.z += frontBack * crossStreet; // nudge cars forward back for variation
    vec2 carDist = Car(rep, uniqueID); // car distance function

    // Drop the cars in the scene with materials
    distAndMat = matmin(distAndMat, carDist);

    return distAndMat;
}

// This basically makes a procedural texture map for the sides of the buildings.
// It makes a texture, a normal for normal mapping, and a mask for window reflection.
void CalcWindows(vec2 block, vec3 pos, inout vec3 texColor, inout float windowRef, inout vec3 normal)
{
    vec3 hue = vec3(Hash21(block)*0.8, Hash21(block*7.89)*0.4, Hash21(block*37.89)*0.5);
    texColor += hue*0.4;
    texColor *= 0.75;
    float window = 0.0;
    window = max(window, mix(0.2, 1.0, floor(fract(pos.y*20.0-0.35)*2.0+0.1)));
    if (pos.y < 0.05) window = 1.0;
    float winWidth = Hash21(block*4.321)*2.0;
    if ((winWidth < 1.3) && (winWidth >= 1.0)) winWidth = 1.3;
    window = max(window, mix(0.2, 1.0, floor(fract(pos.x * 40.0+0.05)*winWidth)));
    window = max(window, mix(0.2, 1.0, floor(fract(pos.z * 40.0+0.05)*winWidth)));
    if (window < 0.5)
    {
        windowRef += 1.0;
    }
    window *= Hash21(block*1.123);
    texColor *= window;

    float wave = floor(sin((pos.y*40.0-0.1)*PI)*0.505-0.5)+1.0;
    normal.y -= max(-1.0, min(1.0, -wave*0.5));
    float pits = min(1.0, abs(sin((pos.z*80.0)*PI))*4.0)-1.0;
    normal.z += pits*0.25;
    pits = min(1.0, abs(sin((pos.x*80.0)*PI))*4.0)-1.0;
    normal.x += pits*0.25;
}

// Input is UV coordinate of pixel to render.
// Output is RGB color.
vec3 RayTrace(in vec2 fragCoord )
{
    marchCount = 0.0;
	// -------------------------------- animate ---------------------------------------
    sunCol = vec3(258.0, 248.0, 200.0) / 3555.0;
	sunDir = normalize(vec3(0.93, 1.0, 1.0));
    horizonCol = vec3(1.0, 0.95, 0.85)*0.9;
    skyCol = vec3(0.3,0.5,0.95);
    exposure = 1.0;
    fade = 1.0;

	vec3 camPos, camUp, camLookat;
	// ------------------- Set up the camera rays for ray marching --------------------
    // Map uv to [-1.0..1.0]
	vec2 uv = fragCoord.xy/iResolution.xy * 2.0 - 1.0;
    uv /= 2.0;  // zoom in

#ifdef MANUAL_CAMERA
    // Camera up vector.
	camUp=vec3(0,1,0);

	// Camera lookat.
	camLookat=vec3(0,0.0,0);

    // debugging camera
    float mx=-iMouse.x/iResolution.x*PI*2.0;// + localTime * 0.05;
	float my=iMouse.y/iResolution.y*3.14*0.5 + PI/2.0;// + sin(localTime * 0.3)*0.8+0.1;//*PI/2.01;
	camPos = vec3(cos(my)*cos(mx),sin(my),cos(my)*sin(mx))*7.35;//7.35
#else
    // Do the camera fly-by animation and different scenes.
    // Time variables for start and end of each scene
    const float t0 = 0.0;
    const float t1 = 8.0;
    const float t2 = 14.0;
    const float t3 = 24.0;
    const float t4 = 38.0;
    const float t5 = 56.0;
    const float t6 = 58.0;
    /*const float t0 = 0.0;
    const float t1 = 0.0;
    const float t2 = 0.0;
    const float t3 = 0.0;
    const float t4 = 0.0;
    const float t5 = 16.0;
    const float t6 = 18.0;*/
    // Repeat the animation after time t6
    localTime = fract(localTime / t6) * t6;
    if (localTime < t1)
    {
        float time = localTime - t0;
        float alpha = time / (t1 - t0);
        fade = saturate(time);
        fade *= saturate(t1 - localTime);
        camPos = vec3(13.0, 3.3, -3.5);
        camPos.x -= smoothstep(0.0, 1.0, alpha) * 4.8;
        camUp=vec3(0,1,0);
        camLookat=vec3(0,1.5,1.5);
    } else if (localTime < t2)
    {
        float time = localTime - t1;
        float alpha = time / (t2 - t1);
        fade = saturate(time);
        fade *= saturate(t2 - localTime);
        camPos = vec3(26.0, 0.05+smoothstep(0.0, 1.0, alpha)*0.4, 2.0);
        camPos.z -= alpha * 2.8;
        camUp=vec3(0,1,0);
        camLookat=vec3(camPos.x-0.3,-8.15,-40.0);
        
        sunDir = normalize(vec3(0.95, 0.6, 1.0));
        sunCol = vec3(258.0, 248.0, 160.0) / 3555.0;
        exposure *= 0.7;
        skyCol *= 1.5;
    } else if (localTime < t3)
    {
        float time = localTime - t2;
        float alpha = time / (t3 - t2);
        fade = saturate(time);
        fade *= saturate(t3 - localTime);
        camPos = vec3(12.0, 6.3, -0.5);
        camPos.y -= alpha * 5.5;
        camPos.x = cos(alpha*1.0) * 5.2;
        camPos.z = sin(alpha*1.0) * 5.2;
        camUp=normalize(vec3(0,1,-0.5 + alpha * 0.5));
        camLookat=vec3(0,1.0,-0.5);
    } else if (localTime < t4)
    {
        float time = localTime - t3;
        float alpha = time / (t4 - t3);
        fade = saturate(time);
        fade *= saturate(t4 - localTime);
        camPos = vec3(2.15-alpha*0.5, 0.02, -1.0-alpha*0.2);
        camPos.y += smoothstep(0.0,1.0,alpha*alpha) * 3.4;
        camUp=normalize(vec3(0,1,0.0));
        camLookat=vec3(0,0.5+alpha,alpha*5.0);
    } else if (localTime < t5)
    {
        float time = localTime - t4;
        float alpha = time / (t5 - t4);
        fade = saturate(time);
        fade *= saturate(t5 - localTime);
        camPos = vec3(-2.0, 1.3- alpha*1.2, -10.5-alpha*0.5);
        camUp=normalize(vec3(0,1,0.0));
        camLookat=vec3(-2.0,0.3+alpha,-0.0);
        sunDir = normalize(vec3(0.5-alpha*0.6, 0.3-alpha*0.3, 1.0));
        sunCol = vec3(258.0, 148.0, 60.0) / 3555.0;
        localTime *= 16.0;
        exposure *= 0.4;
        horizonCol = vec3(1.0, 0.5, 0.35)*2.0;
        skyCol = vec3(0.75,0.5,0.95);

    } else if (localTime < t6)
    {
        fade = 0.0;
        camPos = vec3(26.0, 100.0, 2.0);
        camUp=vec3(0,1,0);
        camLookat=vec3(0.3,0.15,0.0);
    }
#endif

	// Camera setup for ray tracing / marching
	vec3 camVec=normalize(camLookat - camPos);
	vec3 sideNorm=normalize(cross(camUp, camVec));
	vec3 upNorm=cross(camVec, sideNorm);
	vec3 worldFacing=(camPos + camVec);
	vec3 worldPix = worldFacing + uv.x * sideNorm * (iResolution.x/iResolution.y) + uv.y * upNorm;
	vec3 rayVec = normalize(worldPix - camPos);

	// ----------------------------- Ray march the scene ------------------------------
	vec2 distAndMat;  // Distance and material
	float t = 0.05;// + Hash2d(uv)*0.1;	// random dither-fade things close to the camera
	const float maxDepth = 45.0; // farthest distance rays will travel
	vec3 pos = vec3(0.0);
    const float smallVal = 0.000625;
	// ray marching time
    for (int i = 0; i < 250; i++)	// This is the count of the max times the ray actually marches.
    {
        marchCount+=1.0;
        // Step along the ray.
        pos = (camPos + rayVec * t);
        // This is _the_ function that defines the "distance field".
        // It's really what makes the scene geometry. The idea is that the
        // distance field returns the distance to the closest object, and then
        // we know we are safe to "march" along the ray by that much distance
        // without hitting anything. We repeat this until we get really close
        // and then break because we have effectively hit the object.
        distAndMat = DistanceToObject(pos);

        // 2d voxel walk through the city blocks.
        // The distance function is not continuous at city block boundaries,
        // so we have to pause our ray march at each voxel boundary.
        float walk = distAndMat.x;
        float dx = -fract(pos.x);
        if (rayVec.x > 0.0) dx = fract(-pos.x);
        float dz = -fract(pos.z);
        if (rayVec.z > 0.0) dz = fract(-pos.z);
        float nearestVoxel = min(fract(dx/rayVec.x), fract(dz/rayVec.z))+voxelPad;
        nearestVoxel = max(voxelPad, nearestVoxel);// hack that assumes streets and sidewalks are this wide.
        //nearestVoxel = max(nearestVoxel, t * 0.02); // hack to stop voxel walking in the distance.
        walk = min(walk, nearestVoxel);

        // move down the ray a safe amount
        t += walk;
        // If we are very close to the object, let's call it a hit and exit this loop.
        if ((t > maxDepth) || (abs(distAndMat.x) < smallVal)) break;
    }

    // Ray trace a ground plane to infinity
    float alpha = -camPos.y / rayVec.y;
    if ((t > maxDepth) && (rayVec.y < -0.0))
    {
        pos.xz = camPos.xz + rayVec.xz * alpha;
        pos.y = -0.0;
        t = alpha;
        distAndMat.y = 0.0;
        distAndMat.x = 0.0;
    }
	// --------------------------------------------------------------------------------
	// Now that we have done our ray marching, let's put some color on this geometry.
	vec3 finalColor = vec3(0.0);

	// If a ray actually hit the object, let's light it.
    if ((t <= maxDepth) || (t == alpha))
	{
        float dist = distAndMat.x;
        // calculate the normal from the distance field. The distance field is a volume, so if you
        // sample the current point and neighboring points, you can use the difference to get
        // the normal.
        vec3 smallVec = vec3(smallVal, 0, 0);
        vec3 normalU = vec3(dist - DistanceToObject(pos - smallVec.xyy).x,
                           dist - DistanceToObject(pos - smallVec.yxy).x,
                           dist - DistanceToObject(pos - smallVec.yyx).x);
        vec3 normal = normalize(normalU);

        // calculate 2 ambient occlusion values. One for global stuff and one
        // for local stuff
        float ambientS = 1.0;
        ambientS *= saturate(DistanceToObject(pos + normal * 0.0125).x*80.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.025).x*40.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.05).x*20.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.1).x*10.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.2).x*5.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.4).x*2.5);
        //ambientS *= saturate(DistanceToObject(pos + normal * 0.8).x*1.25);
        float ambient = ambientS;// * saturate(DistanceToObject(pos + normal * 1.6).x*1.25*0.5);
        //ambient *= saturate(DistanceToObject(pos + normal * 3.2)*1.25*0.25);
        //ambient *= saturate(DistanceToObject(pos + normal * 6.4)*1.25*0.125);
        ambient = max(0.025, pow(ambient, 0.5));	// tone down ambient with a pow and min clamp it.
        ambient = saturate(ambient);

        // calculate the reflection vector for highlights
        vec3 ref = reflect(rayVec, normal);

        // Trace a ray toward the sun for sun shadows
        float sunShadow = 1.0;
        float iter = 0.01;
        vec3 nudgePos = pos + normal*0.002;	// don't start tracing too close or inside the object
		for (int i = 0; i < 40; i++)
        {
            vec3 shadowPos = nudgePos + sunDir * iter;
            float tempDist = DistanceToObject(shadowPos).x;
	        sunShadow *= saturate(tempDist*150.0);	// Shadow hardness
            if (tempDist <= 0.0) break;

            float walk = tempDist;
            float dx = -fract(shadowPos.x);
            if (sunDir.x > 0.0) dx = fract(-shadowPos.x);
            float dz = -fract(shadowPos.z);
            if (sunDir.z > 0.0) dz = fract(-shadowPos.z);
            float nearestVoxel = min(fract(dx/sunDir.x), fract(dz/sunDir.z))+smallVal;
            nearestVoxel = max(0.2, nearestVoxel);// hack that assumes streets and sidewalks are this wide.
            walk = min(walk, nearestVoxel);

            iter += max(0.01, walk);
            if (iter > 4.5) break;
        }
        sunShadow = saturate(sunShadow);

        // make a few frequencies of noise to give it some texture
        float n =0.0;
        n += noise(pos*32.0);
        n += noise(pos*64.0);
        n += noise(pos*128.0);
        n += noise(pos*256.0);
        n += noise(pos*512.0);
        n = mix(0.7, 0.95, n);

        // ------ Calculate texture color  ------
        vec2 block = floor(pos.xz);
        vec3 texColor = vec3(0.95, 1.0, 1.0);
        texColor *= 0.8;
        float windowRef = 0.0;
        // texture map the sides of buildings
        if ((normal.y < 0.1) && (distAndMat.y == 0.0))
        {
            vec3 posdx = dFdx(pos);
            vec3 posdy = dFdy(pos);
            vec3 posGrad = posdx * Hash21(uv) + posdy * Hash21(uv*7.6543);

            // Quincunx antialias the building texture and normal map.
            // I guess procedural textures are hard to mipmap.
            vec3 colTotal = vec3(0.0);
            vec3 colTemp = texColor;
            vec3 nTemp = vec3(0.0);
            CalcWindows(block, pos, colTemp, windowRef, nTemp);
            colTotal = colTemp;

            colTemp = texColor;
            CalcWindows(block, pos + posdx * 0.666, colTemp, windowRef, nTemp);
            colTotal += colTemp;

            colTemp = texColor;
            CalcWindows(block, pos + posdx * 0.666 + posdy * 0.666, colTemp, windowRef, nTemp);
            colTotal += colTemp;

            colTemp = texColor;
            CalcWindows(block, pos + posdy * 0.666, colTemp, windowRef, nTemp);
            colTotal += colTemp;

            colTemp = texColor;
            CalcWindows(block, pos + posdx * 0.333 + posdy * 0.333, colTemp, windowRef, nTemp);
            colTotal += colTemp;

            texColor = colTotal * 0.2;
            windowRef *= 0.2;

            normal = normalize(normal + nTemp * 0.2);
        }
        else
        {
            // Draw the road
            float xroad = abs(fract(pos.x+0.5)-0.5);
            float zroad = abs(fract(pos.z+0.5)-0.5);
            float road = saturate((min(xroad, zroad)-0.143)*480.0);
            texColor *= 1.0-normal.y*0.95*Hash21(block*9.87)*road; // change rooftop color
            texColor *= mix(0.1, 1.0, road);

            // double yellow line in middle of road
            float yellowLine = saturate(1.0-(min(xroad, zroad)-0.002)*480.0);
            yellowLine *= saturate((min(xroad, zroad)-0.0005)*480.0);
            yellowLine *= saturate((xroad*xroad+zroad*zroad-0.05)*880.0);
            texColor = mix(texColor, vec3(1.0, 0.8, 0.3), yellowLine);

            // white dashed lines on road
            float whiteLine = saturate(1.0-(min(xroad, zroad)-0.06)*480.0);
            whiteLine *= saturate((min(xroad, zroad)-0.056)*480.0);
            whiteLine *= saturate((xroad*xroad+zroad*zroad-0.05)*880.0);
            whiteLine *= saturate(1.0-(fract(zroad*8.0)-0.5)*280.0);  // dotted line
            whiteLine *= saturate(1.0-(fract(xroad*8.0)-0.5)*280.0);
            texColor = mix(texColor, vec3(0.5), whiteLine);

            whiteLine = saturate(1.0-(min(xroad, zroad)-0.11)*480.0);
            whiteLine *= saturate((min(xroad, zroad)-0.106)*480.0);
            whiteLine *= saturate((xroad*xroad+zroad*zroad-0.06)*880.0);
            texColor = mix(texColor, vec3(0.5), whiteLine);

            // crosswalk
            float crossWalk = saturate(1.0-(fract(xroad*40.0)-0.5)*280.0);
            crossWalk *= saturate((zroad-0.15)*880.0);
            crossWalk *= saturate((-zroad+0.21)*880.0)*(1.0-road);
            crossWalk *= n*n;
            texColor = mix(texColor, vec3(0.25), crossWalk);
            crossWalk = saturate(1.0-(fract(zroad*40.0)-0.5)*280.0);
            crossWalk *= saturate((xroad-0.15)*880.0);
            crossWalk *= saturate((-xroad+0.21)*880.0)*(1.0-road);
            crossWalk *= n*n;
            texColor = mix(texColor, vec3(0.25), crossWalk);

            {
                // sidewalk cracks
                float sidewalk = 1.0;
                vec2 blockSize = vec2(100.0);
                if (pos.y > 0.1) blockSize = vec2(10.0, 50);
                //sidewalk *= pow(abs(sin(pos.x*blockSize)), 0.025);
                //sidewalk *= pow(abs(sin(pos.z*blockSize)), 0.025);
                sidewalk *= saturate(abs(sin(pos.z*blockSize.x)*800.0/blockSize.x));
                sidewalk *= saturate(abs(sin(pos.x*blockSize.y)*800.0/blockSize.y));
                sidewalk = saturate(mix(0.7, 1.0, sidewalk));
                sidewalk = saturate((1.0-road) + sidewalk);
                texColor *= sidewalk;
            }
        }
        // Car tires are almost black to not call attention to their ugly.
        if (distAndMat.y == 3.0)
        {
            texColor = vec3(0.05);
        }

        // apply noise
        texColor *= vec3(1.0)*n*0.05;
        texColor *= 0.7;
        texColor = saturate(texColor);

        float windowMask = 0.0;
        if (distAndMat.y >= 100.0)
        {
            // car texture and windows
            texColor = vec3(Hash11(distAndMat.y)*1.0, Hash11(distAndMat.y*8.765), Hash11(distAndMat.y*17.731))*0.1;
            texColor = pow(abs(texColor), vec3(0.2));  // bias toward white
            texColor = max(vec3(0.25), texColor);  // not too saturated color.
            texColor.z = min(texColor.y, texColor.z);  // no purple cars. just not realistic. :)
            texColor *= Hash11(distAndMat.y*0.789) * 0.15;
            windowMask = saturate( max(0.0, abs(pos.y - 0.0175)*3800.0)-10.0);
            vec2 dirNorm = abs(normalize(normal.xz));
            float pillars = saturate(1.0-max(dirNorm.x, dirNorm.y));
            pillars = pow(max(0.0, pillars-0.15), 0.125);
            windowMask = max(windowMask, pillars);
            texColor *= windowMask;
        }

        // ------ Calculate lighting color ------
        // Start with sun color, standard lighting equation, and shadow
        vec3 lightColor = vec3(100.0)*sunCol * saturate(dot(sunDir, normal)) * sunShadow;
        // weighted average the near ambient occlusion with the far for just the right look
        float ambientAvg = (ambient*3.0 + ambientS) * 0.25;
        // Add sky color with ambient acclusion
        lightColor += (skyCol * saturate(normal.y *0.5+0.5))*pow(ambientAvg, 0.35)*2.5;
        lightColor *= 4.0;

        // finally, apply the light to the texture.
        finalColor = texColor * lightColor;
        // Reflections for cars
        if (distAndMat.y >= 100.0)
        {
            float yfade = max(0.01, min(1.0, ref.y*100.0));
            // low-res way of making lines at the edges of car windows. Not sure I like it.
            yfade *= (saturate(1.0-abs(dFdx(windowMask)*dFdy(windowMask))*250.995));
            finalColor += GetEnvMapSkyline(ref, sunDir, pos.y-1.5)*0.3*yfade*max(0.4,sunShadow);
            //finalColor += saturate(texture(iChannel0, ref).xyz-0.35)*0.15*max(0.2,sunShadow);
        }
        // reflections for building windows
        if (windowRef != 0.0)
        {
            finalColor *= mix(1.0, 0.6, windowRef);
            float yfade = max(0.01, min(1.0, ref.y*100.0));
            finalColor += GetEnvMapSkyline(ref, sunDir, pos.y-0.5)*0.6*yfade*max(0.6,sunShadow)*windowRef;//*(windowMask*0.5+0.5);
            //finalColor += saturate(texture(iChannel0, ref).xyz-0.35)*0.15*max(0.25,sunShadow)*windowRef;
        }
        finalColor *= 0.9;
        // fog that fades to reddish plus the sun color so that fog is brightest towards sun
        vec3 rv2 = rayVec;
        rv2.y *= saturate(sign(rv2.y));
        vec3 fogColor = GetEnvMap(rv2, sunDir);
        fogColor = min(vec3(9.0), fogColor);
        finalColor = mix(fogColor, finalColor, exp(-t*0.02));

        // visualize length of gradient of distance field to check distance field correctness
        //finalColor = vec3(0.5) * (length(normalU) / smallVec.x);
        //finalColor = vec3(marchCount)/255.0;
	}
    else
    {
        // Our ray trace hit nothing, so draw sky.
        finalColor = GetEnvMap(rayVec, sunDir);
    }

    // vignette?
    finalColor *= vec3(1.0) * saturate(1.0 - length(uv/2.5));
    finalColor *= 1.3*exposure;

	// output the final color without gamma correction - will do gamma later.
	return vec3(clamp(finalColor, 0.0, 1.0)*saturate(fade+0.2));
}

#ifdef NON_REALTIME_HQ_RENDER
// This function breaks the image down into blocks and scans
// through them, rendering 1 block at a time. It's for non-
// realtime things that take a long time to render.

// This is the frame rate to render at. Too fast and you will
// miss some blocks.
const float blockRate = 20.0;
void BlockRender(in vec2 fragCoord)
{
    // blockSize is how much it will try to render in 1 frame.
    // adjust this smaller for more complex scenes, bigger for
    // faster render times.
    const float blockSize = 64.0;
    // Make the block repeatedly scan across the image based on time.
    float frame = floor(iTime * blockRate);
    vec2 blockRes = floor(iResolution.xy / blockSize) + vec2(1.0);
    // ugly bug with mod.
    //float blockX = mod(frame, blockRes.x);
    float blockX = fract(frame / blockRes.x) * blockRes.x;
    //float blockY = mod(floor(frame / blockRes.x), blockRes.y);
    float blockY = fract(floor(frame / blockRes.x) / blockRes.y) * blockRes.y;
    // Don't draw anything outside the current block.
    if ((fragCoord.x - blockX * blockSize >= blockSize) ||
    	(fragCoord.x - (blockX - 1.0) * blockSize < blockSize) ||
    	(fragCoord.y - blockY * blockSize >= blockSize) ||
    	(fragCoord.y - (blockY - 1.0) * blockSize < blockSize))
    {
        discard;
    }
}
#endif

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
#ifdef NON_REALTIME_HQ_RENDER
    // Optionally render a non-realtime scene with high quality
    BlockRender(fragCoord);
#endif

    // Do a multi-pass render
    vec3 finalColor = vec3(0.0);
#ifdef NON_REALTIME_HQ_RENDER
    for (float i = 0.0; i < antialiasingSamples; i++)
    {
        const float motionBlurLengthInSeconds = 1.0 / 60.0;
        // Set this to the time in seconds of the frame to render.
	    localTime = frameToRenderHQ;
        // This line will motion-blur the renders
        localTime += Hash11(v21(fragCoord + seed)) * motionBlurLengthInSeconds;
        // Jitter the pixel position so we get antialiasing when we do multiple passes.
        vec2 jittered = fragCoord.xy + vec2(
            Hash21(fragCoord + seed),
            Hash21(fragCoord*7.234567 + seed)
            );
        // don't antialias if only 1 sample.
        if (antialiasingSamples == 1.0) jittered = fragCoord;
        // Accumulate one pass of raytracing into our pixel value
	    finalColor += RayTrace(jittered);
        // Change the random seed for each pass.
	    seed *= 1.01234567;
    }
    // Average all accumulated pixel intensities
    finalColor /= antialiasingSamples;
#else
    // Regular real-time rendering
    localTime = iTime;
    finalColor = RayTrace(fragCoord);
#endif

    fragColor = vec4(sqrt(clamp(finalColor, 0.0, 1.0)),1.0);
}
  ''',
  },
  {
    // https://www.shadertoy.com/view/XlfGRj
    'url': 'https://www.shadertoy.com/view/XlfGRj',
    'fragment': '''
// Star Nest by Pablo Roman Andrioli


#define iterations 17
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom   0.800
#define tile   0.850
#define speed  0.010 

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	//get coords and direction
	vec2 uv=fragCoord.xy/iResolution.xy-.5;
	uv.y*=iResolution.y/iResolution.x;
	vec3 dir=vec3(uv*zoom,1.);
	float time=iTime*speed+.25;

	//mouse rotation
	float a1=.5+iMouse.x/iResolution.x*2.;
	float a2=.8+iMouse.y/iResolution.y*2.;
	mat2 rot1=mat2(cos(a1),sin(a1),-sin(a1),cos(a1));
	mat2 rot2=mat2(cos(a2),sin(a2),-sin(a2),cos(a2));
	dir.xz*=rot1;
	dir.xy*=rot2;
	vec3 from=vec3(1.,.5,0.5);
	from+=vec3(time*2.,time,-2.);
	from.xz*=rot1;
	from.xy*=rot2;
	
	//volumetric rendering
	float s=0.1,fade=1.;
	vec3 v=vec3(0.);
	for (int r=0; r<volsteps; r++) {
		vec3 p=from+s*dir*.5;
		p = abs(vec3(tile)-mod(p,vec3(tile*2.))); // tiling fold
		float pa,a=pa=0.;
		for (int i=0; i<iterations; i++) { 
			p=abs(p)/dot(p,p)-formuparam; // the magic formula
			a+=abs(length(p)-pa); // absolute sum of average change
			pa=length(p);
		}
		float dm=max(0.,darkmatter-a*a*.001); //dark matter
		a*=a*a; // add contrast
		if (r>6) fade*=1.-dm; // dark matter, don't render near
		//v+=vec3(dm,dm*.5,0.);
		v+=fade;
		v+=vec3(s,s*s,s*s*s*s)*a*brightness*fade; // coloring based on distance
		fade*=distfading; // distance fading
		s+=stepsize;
	}
	v=mix(vec3(length(v)),v,saturation); //color adjust
	fragColor = vec4(v*.01,1.);	
	
}
  ''',
  },
  {
    // https://www.shadertoy.com/view/Xs2XDV
    'url': 'https://www.shadertoy.com/view/Xs2XDV',
    'fragment': '''
#define resolution (iResolution.xy)
#define t (iTime * .3)

// Simplest Lambert+Phong material shading model parameters
struct mat_t {
    vec3 diffuse;
    vec4 specular;
};

// There are two separate distance fields, each consists of 6 metaballs and a plane.
#define N 6
vec3 b1[N];
vec3 b2[N];

// Four lights with their positions and colors
vec3 l1pos = 1.*vec3(6., 3., -7.);
vec3 l2pos = 1.*vec3(-7., 1., 6.);
vec3 l3pos = 1.*vec3(7., -4., 7.);
vec3 l4pos = vec3(0.,5.,25.);
vec3 l1color = 40.*vec3(1.,.7,.7);
vec3 l2color = 20.*vec3(.7,.7,1.);
vec3 l3color = 60.*vec3(.7,1.,.7);
vec3 l4color = vec3(100.);

// Distance function approximation for the first field only
float t1(vec3 p) {
    float v = 0.;
    for (int i = 0; i < N; ++i) {
        vec3 b = p - b1[i];
        // metaball field used here is a simple sum of inverse-square distances to metaballs centers
        // all numeric constants are empirically tuned
        v += 5. / dot(b, b);
    }
    // add top y=12 (red) plane
    float d = 12. - p.y; v += 3. / (d*d);
    return v;
}

// Second field distance function is basically the same, but uses b2[] metaballs centers and y=-12 plane
float t2(vec3 p) {
    float v = 0.;
	for (int i = 0; i < N; ++i) {
        vec3 b = p - b2[i];
        v += 5. / dot(b, b);
    }
    float d = 12. + p.y; v += 3. / (d*d);
    return v;
}

// "Repulsive" distance functions which account for both fields
float w1(vec3 p) { return 1. - t1(p) + t2(p); }
float w2(vec3 p) { return 1. + t1(p) - t2(p); }

// Combined world function that picks whichever field is the closest one
float world(vec3 p) {
    return min(w1(p), w2(p));
}

vec3 normal(vec3 p) {
    vec2 e = vec2(.001,0.);
    return normalize(vec3(
        world(p+e.xyy) - world(p-e.xyy),
        world(p+e.yxy) - world(p-e.yxy),
        world(p+e.yyx) - world(p-e.yyx)));
}

// Material is picked based on which field's distance function is the smallest
mat_t material(vec3 p) {
    mat_t m;
    m.diffuse = vec3(0.);
    m.specular = vec4(0.);
    if (w1(p) < w2(p)) {
        m.diffuse = vec3(.92, .027, .027);
        m.specular = vec4(mix(vec3(1.),m.diffuse,.7), 2000.);
    } else {
        m.diffuse = vec3(.14, .17, 1.57);
	    m.specular = vec4(mix(vec3(1.),m.diffuse,.5), 100.);
    }
    return m;
}

float iter = 0.;

// Raymarcher
float trace(in vec3 O, in vec3 D, in float L, in float Lmax) {
    // The usual "step along the ray" loop
    float Lp = L;
    for (int i = 0; i < 40; ++i) {
        iter = float(i);
        float d = world(O + D * L);
        if (d < .01*L || L > Lmax) break;
        // Store previous point (see below)
        Lp = L;

        // Overstep a bit to find intersetion faster (metaball fields aren't cheap)
        L += d * 1.5;
    }

    if (L < Lmax) {
        // Binary search for more exact intersestion position
        // Needed to fix artifacts due to overstep
        for (int i = 0; i < 5; ++i) {
            float Lm = (Lp + L) * .5;
            if (world(O + D * Lm) < .01*Lm) L = Lm; else Lp = Lm;
        }
    }
    iter /= 32.;
    return L;
}

// Simpler tracing function that tries to determine whether the point is in shadow
float shadowtrace(vec3 p, vec3 d, float lmax) {
    float v = .02;
    // Take a few samples between the point and the light position and chech
    // whether they are "inside" geometry
    for (int i = 1; i < 9; ++i) {
        v = min(v, world(p+d*lmax*float(i)/12.));
    }
    return smoothstep(.0, .02, v);

}

// Compute shading color from a single light
vec3 enlight(vec3 p, vec3 v, vec3 n, mat_t m, vec3 lpos, vec3 lcolor) {
    vec3 ldir = lpos - p; // direction from the point to the light source
    float ldist2 = dot(ldir, ldir); // square distance to the light source
    float ldist = sqrt(ldist2); // distance
    ldir /= ldist; // ldir is now normalized
    float shadow = shadowtrace(p, ldir, ldist-.2); // whether the point is in shadow
    return shadow * lcolor * (
        // Diffuse term (simple Lambert)
	  m.diffuse * max(0., dot(n,ldir))
        // Specular term (Phong-ish)
	+ m.specular.rgb * pow(max(0.,dot(normalize(ldir-v), n)), m.specular.w)
        // Specular energy conservation? (or something, it was 3 years ago :D)
        * (m.specular.w + 2.) * (m.specular.w + 4.) / (24. * (m.specular.w + pow(2., -m.specular.w/2.)))
	) / ldist2;
}

// A ridiculous hack to display light sources
// Basic idea is to check whether the ray is "close enough" to the light source position in screen space
// Which is done by calculating the angle between the actual pixel ray and a vector from origin to the light source
vec3 lightball(vec3 lpos, vec3 lcolor, vec3 O, vec3 D, float L) {
    vec3 ldir = lpos-O;
    float ldist = length(ldir);
    if (ldist > L) return vec3(0.);
    float pw = pow(max(0.,dot(normalize(ldir),D)), 20000.);
    return (normalize(lcolor)+vec3(1.)) * pw;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    // Calculate metaball trajectories (all empirical)
    for (int i = 0; i < N; ++i) {
        float fi = float(i)*.7;
        b1[i] = vec3(3.7*sin(t+fi), 1.+10.*cos(t*1.1+fi), 2.3*sin(t*2.3+fi));
        fi = float(i)*1.2;
        b2[i] = vec3(4.4*cos(t*.4+fi),-1.-10.*cos(t*0.7+fi), -2.1*sin(t*1.3+fi));
    }

    vec2 uv = fragCoord.xy / resolution.xy * 2. - 1.;
    uv.x *= resolution.x / resolution.y;

    vec3 O = vec3(0.,0.,20.);
    vec3 D = normalize(vec3(uv,-1.6));

    if (iMouse.z > 0.)
    {
        vec2 m = iMouse.xy/iResolution.xy*2.-1.;
        float a = - m.x * 2. * 3.1415926;
        float s = sin(a), c = cos(a);
        O = vec3(s*20.,-m.y*10.,c*20.);
        vec3 fw = normalize(vec3(0.) - O);
        vec3 rg = cross(fw,vec3(0.,1.,0.));
        D = normalize(mat3(rg, cross(rg, fw), -fw) * D);
    }

    // Raymarch
    float L = trace(O, D, 0., 40.);
    vec3 color = vec3(0.);
    if (L < 40.) {
        // If a hit is detected, then get the position
        vec3 p = O + D * L;
        // And normal
        vec3 n = normal(p);
        // And material
        mat_t m = material(p);

        // Ambient term (I don't think it is visible much)
        color = .001 * m.diffuse * n;

        // Shade for all lights
        color += enlight(p, D, n, m, l1pos, l1color);
        color += enlight(p, D, n, m, l2pos, l2color);
        color += enlight(p, D, n, m, l3pos, l3color);
        color += enlight(p, D, n, m, l4pos, l4color);

        // Fog-like hack-ish attenuation, so the only the center of the scene is visible
        color *= (1. - smoothstep(10., 20., length(p)));
	} else L = 100.;

    // Add light sources hack
    color += lightball(l1pos, l1color, O, D, L);
    color += lightball(l2pos, l2color, O, D, L);
    color += lightball(l3pos, l3color, O, D, L);
    color += lightball(l4pos, l4color, O, D, L);

    // Gamma correct and out
	fragColor = vec4(pow(color,vec3(.7)),1.0);
}
  ''',
  },
  {
    // https://www.shadertoy.com/view/Mss3WN
    'url': 'https://www.shadertoy.com/view/Mss3WN',
    'fragment': '''
/*by mu6k, Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

 I have no idea how I ended up here, but it demosceneish enough to publish.
 You can use the mouse to rotate the camera around the 'object'.
 If you can't see the shadows, increase occlusion_quality.
 If it doesn't compile anymore decrease object_count and render_steps.

 15/06/2013:
 - published
 
 16/06/2013:
 - modified for better performance and compatibility

 muuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuusk!*/

#define occlusion_enabled
#define occlusion_quality 4
//#define occlusion_preview

#define noise_use_smoothstep

#define light_color vec3(0.1,0.4,0.6)
#define light_direction normalize(vec3(.2,1.0,-0.2))
#define light_speed_modifier 1.0

#define object_color vec3(0.9,0.1,0.1)
#define object_count 9
#define object_speed_modifier 1.0

#define render_steps 33

float hash(float x)
{
	return fract(sin(x*.0127863)*17143.321);
}

float hash(vec2 x)
{
	return fract(cos(dot(x.xy,vec2(2.31,53.21))*124.123)*412.0); 
}

vec3 cc(vec3 color, float factor,float factor2) //a wierd color modifier
{
	float w = color.x+color.y+color.z;
	return mix(color,vec3(w)*factor,w*factor2);
}

float hashmix(float x0, float x1, float interp)
{
	x0 = hash(x0);
	x1 = hash(x1);
	#ifdef noise_use_smoothstep
	interp = smoothstep(0.0,1.0,interp);
	#endif
	return mix(x0,x1,interp);
}

float noise(float p) // 1D noise
{
	float pm = mod(p,1.0);
	float pd = p-pm;
	return hashmix(pd,pd+1.0,pm);
}

vec3 rotate_y(vec3 v, float angle)
{
	float ca = cos(angle); float sa = sin(angle);
	return v*mat3(
		+ca, +.0, -sa,
		+.0,+1.0, +.0,
		+sa, +.0, +ca);
}

vec3 rotate_x(vec3 v, float angle)
{
	float ca = cos(angle); float sa = sin(angle);
	return v*mat3(
		+1.0, +.0, +.0,
		+.0, +ca, -sa,
		+.0, +sa, +ca);
}

float max3(float a, float b, float c)//returns the maximum of 3 values
{
	return max(a,max(b,c));
}

vec3 bpos[object_count];//position for each metaball

float dist(vec3 p)//distance function
{
	float d=1024.0;
	float nd;
	for (int i=0 ;i<object_count; i++)
	{
		vec3 np = p+bpos[i];
		float shape0 = max3(abs(np.x),abs(np.y),abs(np.z))-1.0;
		float shape1 = length(np)-1.0;
		nd = shape0+(shape1-shape0)*2.0;
		d = mix(d,nd,smoothstep(-1.0,+1.0,d-nd));
	}
	return d;
}

vec3 normal(vec3 p,float e) //returns the normal, uses the distance function
{
	float d=dist(p);
	return normalize(vec3(dist(p+vec3(e,0,0))-d,dist(p+vec3(0,e,0))-d,dist(p+vec3(0,0,e))-d));
}

vec3 light = light_direction; //global variable that holds light direction

vec3 background(vec3 d)//render background
{
	float t=iTime*0.5*light_speed_modifier;
	float qq = dot(d,light)*.5+.5;
	float bgl = qq;
	float q = (bgl+noise(bgl*6.0+t)*.85+noise(bgl*12.0+t)*.85);
	q+= pow(qq,32.0)*2.0;
	vec3 sky = vec3(0.1,0.4,0.6)*q;
	return sky;
}

float occlusion(vec3 p, vec3 d)//returns how much a point is visible from a given direction
{
	float occ = 1.0;
	p=p+d;
	for (int i=0; i<occlusion_quality; i++)
	{
		float dd = dist(p);
		p+=d*dd;
		occ = min(occ,dd);
	}
	return max(.0,occ);
}

vec3 object_material(vec3 p, vec3 d)
{
	vec3 color = normalize(object_color*light_color);
	vec3 n = normal(p,0.1);
	vec3 r = reflect(d,n);	
	
	float reflectance = dot(d,r)*.5+.5;reflectance=pow(reflectance,2.0);
	float diffuse = dot(light,n)*.5+.5; diffuse = max(.0,diffuse);
	
	#ifdef occlusion_enabled
		float oa = occlusion(p,n)*.4+.6;
		float od = occlusion(p,light)*.95+.05;
		float os = occlusion(p,r)*.95+.05;
	#else
		float oa=1.0;
		float ob=1.0;
		float oc=1.0;
	#endif
	
	#ifndef occlusion_preview
		color = 
		color*oa*.2 + //ambient
		color*diffuse*od*.7 + //diffuse
		background(r)*os*reflectance*.7; //reflection
	#else
		color=vec3((oa+od+os)*.3);
	#endif
	
	return color;
}

#define offset1 4.7
#define offset2 4.6

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy - 0.5;
	uv.x *= iResolution.x/iResolution.y; //fix aspect ratio
	vec3 mouse = vec3(iMouse.xy/iResolution.xy - 0.5,iMouse.z-.5);
	
	float t = iTime*.5*object_speed_modifier + 2.0;
	
	for (int i=0 ;i<object_count; i++) //position for each metaball
	{
		bpos[i] = 1.3*vec3(
			sin(t*0.967+float(i)*42.0),
			sin(t*.423+float(i)*152.0),
			sin(t*.76321+float(i)));
	}
	
	//setup the camera
	vec3 p = vec3(.0,0.0,-4.0);
	p = rotate_x(p,mouse.y*9.0+offset1);
	p = rotate_y(p,mouse.x*9.0+offset2);
	vec3 d = vec3(uv,1.0);
	d.z -= length(d)*.5; //lens distort
	d = normalize(d);
	d = rotate_x(d,mouse.y*9.0+offset1);
	d = rotate_y(d,mouse.x*9.0+offset2);
	
	//and action!
	float dd;
	vec3 color;
	for (int i=0; i<render_steps; i++) //raymarch
	{
		dd = dist(p);
		p+=d*dd*.7;
		if (dd<.04 || dd>4.0) break;
	}
	
	if (dd<0.5) //close enough
		color = object_material(p,d);
	else
		color = background(d);
	
	//post procesing
	color *=.85;
	color = mix(color,color*color,0.3);
	color -= hash(color.xy+uv.xy)*.015;
	color -= length(uv)*.1;
	color =cc(color,.5,.6);
	fragColor = vec4(color,1.0);
}
''',
  },
  {
    // https://www.shadertoy.com/view/3l23Rh
    'url': 'https://www.shadertoy.com/view/3l23Rh',
    'fragment': '''
// Protean clouds by nimitz (twitter: @stormoid)
// https://www.shadertoy.com/view/3l23Rh
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

/*
	Technical details:

	The main volume noise is generated from a deformed periodic grid, which can produce
	a large range of noise-like patterns at very cheap evalutation cost. Allowing for multiple
	fetches of volume gradient computation for improved lighting.

	To further accelerate marching, since the volume is smooth, more than half the the density
	information isn't used to rendering or shading but only as an underlying volume	distance to 
	determine dynamic step size, by carefully selecting an equation	(polynomial for speed) to 
	step as a function of overall density (not necessarily rendered) the visual results can be 
	the	same as a naive implementation with ~40% increase in rendering performance.

	Since the dynamic marching step size is even less uniform due to steps not being rendered at all
	the fog is evaluated as the difference of the fog integral at each rendered step.

*/

mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;
float mag2(vec2 p){return dot(p,p);}
float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }
float prm1 = 0.;
vec2 bsMo = vec2(0);

vec2 disp(float t){ return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.; }

vec2 map(vec3 p)
{
    vec3 p2 = p;
    p2.xy -= disp(p.z).xy;
    p.xy *= rot(sin(p.z+iTime)*(0.1 + prm1*0.05) + iTime*0.09);
    float cl = mag2(p2.xy);
    float d = 0.;
    p *= .61;
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1*0.2;
    for(int i = 0; i < 5; i++)
    {
		p += sin(p.zxy*0.75*trk + iTime*trk*.8)*dspAmp;
        d -= abs(dot(cos(p), sin(p.yzx))*z);
        z *= 0.57;
        trk *= 1.4;
        p = p*m3;
    }
    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y;
    return vec2(d + cl*.2 + 0.25, cl);
}

vec4 render( in vec3 ro, in vec3 rd, float time )
{
	vec4 rez = vec4(0);
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);
	float t = 1.5;
	float fogT = 0.;
	for(int i=0; i<130; i++)
	{
		if(rez.a > 0.99)break;

		vec3 pos = ro + t*rd;
        vec2 mpv = map(pos);
		float den = clamp(mpv.x-0.3,0.,1.)*1.12;
		float dn = clamp((mpv.x + 2.),0.,3.);
        
		vec4 col = vec4(0);
        if (mpv.x > 0.6)
        {
        
            col = vec4(sin(vec3(5.,0.4,0.2) + mpv.y*0.1 +sin(pos.z*0.4)*0.5 + 1.8)*0.5 + 0.5,0.08);
            col *= den*den*den;
			col.rgb *= linstep(4.,-2.5, mpv.x)*2.3;
            float dif =  clamp((den - map(pos+.8).x)/9., 0.001, 1. );
            dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );
            col.xyz *= den*(vec3(0.005,.045,.075) + 1.5*vec3(0.033,0.07,0.03)*dif);
        }
		
		float fogC = exp(t*0.2 - 2.2);
		col.rgba += vec4(0.06,0.11,0.11, 0.1)*clamp(fogC-fogT, 0., 1.);
		fogT = fogC;
		rez = rez + col*(1. - rez.a);
		t += clamp(0.5 - dn*dn*.05, 0.09, 0.3);
	}
	return clamp(rez, 0.0, 1.0);
}

float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//from my "Will it blend" shader (https://www.shadertoy.com/view/lsdGzN)
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    float lgt = dot(vec3(1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
	vec2 q = fragCoord.xy/iResolution.xy;
    vec2 p = (gl_FragCoord.xy - 0.5*iResolution.xy)/iResolution.y;
    bsMo = (iMouse.xy - 0.5*iResolution.xy)/iResolution.y;
    
    float time = iTime*3.;
    vec3 ro = vec3(0,0,time);
    
    ro += vec3(sin(iTime)*0.5,sin(iTime*1.)*0.,0);
        
    float dspAmp = .85;
    ro.xy += disp(ro.z)*dspAmp;
    float tgtDst = 3.5;
    
    vec3 target = normalize(ro - vec3(disp(time + tgtDst)*dspAmp, time + tgtDst));
    ro.x -= bsMo.x*2.;
    vec3 rightdir = normalize(cross(target, vec3(0,1,0)));
    vec3 updir = normalize(cross(rightdir, target));
    rightdir = normalize(cross(updir, target));
	vec3 rd=normalize((p.x*rightdir + p.y*updir)*1. - target);
    rd.xy *= rot(-disp(time + 3.5).x*0.2 + bsMo.x);
    prm1 = smoothstep(-0.4, 0.4,sin(iTime*0.3));
	vec4 scn = render(ro, rd, time);
		
    vec3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    
    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);

    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign
    
	fragColor = vec4( col, 1.0 );
}
''',
  },
  {
    // https://www.shadertoy.com/view/Ms2SD1
    'url': 'https://www.shadertoy.com/view/Ms2SD1',
    'fragment': '''
/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * Contact: tdmaav@gmail.com
 */

const int NUM_STEPS = 8;
const float PI	 	= 3.141592;
const float EPSILON	= 1e-3;
#define EPSILON_NRM (0.1 / iResolution.x)
#define AA

// sea
const int ITER_GEOMETRY = 3;
const int ITER_FRAGMENT = 5;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;
#define SEA_TIME (1.0 + iTime * SEA_SPEED)
const mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
    vec2 a2 = vec2(sin(ang.y),cos(ang.y));
    vec2 a3 = vec2(sin(ang.z),cos(ang.z));
    mat3 m;
    m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

// sea
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_GEOMETRY; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float map_detailed(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_FRAGMENT; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
    vec3 n;
    n.y = map_detailed(p);    
    n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
    float tm = 0.0;
    float tx = 1000.0;    
    float hx = map(ori + dir * tx);
    if(hx > 0.0) {
        p = ori + dir * tx;
        return tx;   
    }
    float hm = map(ori + dir * tm);    
    float tmid = 0.0;
    for(int i = 0; i < NUM_STEPS; i++) {
        tmid = mix(tm,tx, hm/(hm-hx));                   
        p = ori + dir * tmid;                   
    	float hmid = map(p);
		if(hmid < 0.0) {
        	tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }
    }
    return tmid;
}

vec3 getPixel(in vec2 coord, float time) {    
    vec2 uv = coord / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;    
        
    // ray
    vec3 ang = vec3(sin(time*3.0)*0.1,sin(time)*0.2+0.3,time);    
    vec3 ori = vec3(0.0,3.5,time*5.0);
    vec3 dir = normalize(vec3(uv.xy,-2.0)); dir.z += length(uv) * 0.14;
    dir = normalize(dir) * fromEuler(ang);
    
    // tracing
    vec3 p;
    heightMapTracing(ori,dir,p);
    vec3 dist = p - ori;
    vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
    vec3 light = normalize(vec3(0.0,1.0,0.8)); 
             
    // color
    return mix(
        getSkyColor(dir),
        getSeaColor(p,n,light,dir,dist),
    	pow(smoothstep(0.0,-0.02,dir.y),0.2));
}

// main
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float time = iTime * 0.3 + iMouse.x*0.01;
	
#ifdef AA
    vec3 color = vec3(0.0);
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
        	vec2 uv = fragCoord+vec2(i,j)/3.0;
    		color += getPixel(uv, time);
        }
    }
    color /= 9.0;
#else
    vec3 color = getPixel(fragCoord, time);
#endif
    
    // post
	fragColor = vec4(pow(color,vec3(0.65)), 1.0);
}
''',
  },
  {
    // https://www.shadertoy.com/view/csB3zy
    'url': 'https://www.shadertoy.com/view/csB3zy',
    'fragment': '''
// Copyright Inigo Quilez, 2022 - https://iquilezles.org/
// I am the sole copyright owner of this Work. You cannot host, display, distribute
// or share this Work neither as it is or altered, on Shadertoy or anywhere else,
// in any form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot sell this
// Work, mint an NFTs of it or train a neural network with it without my permission.
// I share this Work for educational purposes, and you can link to it through an URL
// with proper attribution and unmodified art. If these conditions are too
// restrictive contact me and we'll definitely work it out.


// I made this after I saw leon's shader for the Inercia Shader Royale 2022
// (https://www.shadertoy.com/view/md2GDD), not because of any particular reason
// (KIFS fractals are very popular and I've done my share
// https://www.shadertoy.com/view/lssGRM and https://www.shadertoy.com/view/4sX3R2)
// but because sometimes you just see something pretty and want to attempt doing
// something pretty too.
//
// I'm using smooth-minimum and smooth-abs to blend all spheres together into a
// single organic shape. The rest is color tweaking.
//
// The shader runs slow because I'm doing volumetric rendering so I get some sweet
// transparencies. With regular raymarching this runs at 60 fps in a full screen,
// but as usual I'm okey with doing something 4x slower if it looks 10% prettier!


// https://iquilezles.org/articles/intersectors/
vec2 sphIntersect( in vec3 ro, in vec3 rd, float ra )
{
	float b = dot( ro, rd );
	float c = dot( ro, ro ) - ra*ra;
	float h = b*b - c;
	if( h<0.0 ) return vec2(-1.0);
    h = sqrt(h);
	return vec2(-b-h,-b+h);
}

// https://iquilezles.org/articles/smin/
vec4 smin( vec4 a, vec4 b, float k )
{
    float h = max( k-abs(a.x-b.x), 0.0 )/k;
    float m = h*h*0.5;
    float s = m*k*0.5;
    vec2 r = (a.x<b.x) ? vec2(a.x,m) : vec2(b.x,1.0-m);
    return vec4(r.x-s, mix( a.yzw, b.yzw, r.y ) );
}

// https://iquilezles.org/articles/functions/
float sabs( float x, float k )
{
    return sqrt(x*x+k);
}

vec2 rot( vec2 p, float a )
{
    float co = cos(a);
    float si = sin(a);
    return mat2(co,-si,si,co) * p;
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(0.0, cos(cr),sin(cr));
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv =          ( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

//======================================================================
// creature
//======================================================================

// euclidean distance to creature, and color at closest point
vec4 map( in vec3 p, float time )
{
    float d1 = 0.10*sin(-time*6.283185/8.0 +  3.0*p.y); // animate body
    float d2 = 0.05*sin( time*10.0         + 60.0*p.y); // animate flagellum

    vec4 dcol = vec4(1e20,0.0,0.0,0.0);
    float sc = 1.0;
    for( int i=0; i<20; i++ )
    {
        // rotate coords
        p.xz = rot(p.xz, 17.0*sc + d1*smoothstep( 5.0, 1.0,float(i))); // body
        p.yz = rot(p.yz, -1.0*sc + d2*smoothstep(10.0,12.0,float(i))); // flagellum
        
        // smooth mirror and translate coords
        p.x = sabs(p.x,0.0001*sc) - 0.22*sc;

        // distance
        float d = (i==19) ? length(p*vec3(1.0,1.0,0.1)) - 0.1*sc :
                            length(p) - (0.1*sc + 0.001*sc*sin(2000.0*p.y*sc));
        // color
        vec3 c = (i==9) ? vec3(0.75) :
                          vec3(0.4,0.2,0.2) + vec3(0.1,0.5,0.6)*float(i)/20.0 + 0.1*cos(vec3(0,1,2)-p*10.0);

        // blend in distance and color
        dcol = smin(dcol, vec4(d,c), 0.12*sc);
    
        // scale coords for next iteration
        sc /= 1.2;
    }
    
    return dcol;
}

// https://iquilezles.org/articles/nvscene2008/rwwtt.pdf
float calcAO( in vec3 pos, in vec3 nor, in float time )
{
	float occ = 0.0;
    for( int i=0; i<8; i++ )
    {
        float h = 0.01 + 0.4*float(i)/7.0;
        vec3  w = normalize( nor + normalize(sin(float(i)+vec3(0,2,4))));
        float d = map( pos + h*w, time ).x;
        occ += h-d;
    }
    return clamp( 1.0 - 0.71*occ, 0.0, 1.0 );
}

// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal( in vec3 pos, float dis, in float time )
{
    const vec2 e = vec2(0.001,0.0);
    return normalize( vec3( map( pos + e.xyy, time ).x,
                            map( pos + e.yxy, time ).x,
                            map( pos + e.yyx, time ).x)-dis );
}

//======================================================================
// CITA - Crap In The Air
//======================================================================

// a 3D dithered grid of spheres
vec4 mapCITA( in vec3 pos, in float time )
{
    pos.y += time*0.02;

    const float rep = 1.5;
    vec3 ip = floor(pos/rep);
    vec3 fp = fract(pos/rep);
    vec3 op = vec3( (fp.x<0.0)?-1.0:0.0, (fp.y<0.0)?-1.0:0.0, (fp.z<0.0)?-1.0:0.0 );
    
    // note we only need to check 8 cells, not 27
    vec4 dr = vec4(1e20);
    for( int i=0; i<2; i++ )
    for( int j=0; j<2; j++ )
    for( int k=0; k<2; k++ )
    {
        vec3 b = vec3( float(i), float(j), float(k) );
        vec3 id = ip + b + op;
        
        // random location per sphere
        vec3 ra = fract(sin(dot(id,vec3(1,123,1231))+vec3(0,1,2))*vec3(338.5453123,278.1459123,191.1234));
        vec3 o = 0.3*sin(6.283185*time/48.0 + 50.0*ra);
        vec3 r = b - fp + o;
        
        float d = dot(r,r);
        if( d<dr.x ) dr = vec4(d,r);
    }
    return vec4(sqrt(dr.x)*rep-0.02,dr.yzw);
}

// https://iquilezles.org/articles/raymarchingdf/
vec4 raycastCITA( in vec3 ro, in vec3 rd, in float px, in float tmax, in float time )
{
    float t = 0.0;
    vec3 res = vec3(0.0);
	for( int i=0; i<256; i++ )
	{
        vec3 pos = ro + t*rd;
        vec4 h = mapCITA( pos, time );
        res = h.yzw;
        if( h.x<0.0005*px*t || t>tmax ) break;
		t += h.x;
	}
	return (t<tmax) ? vec4(t,res) : vec4(-1.0);
}

//======================================================================
// rendering
//======================================================================

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // coordinates
    vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    vec2 mo = iMouse.xy/iResolution.xy;
    
    float time = mod( iTime, 48.0 );
    
    // camera location and lense
    vec3  ta = 0.08*sin( 6.283185*time/24.0+vec3(0,2,4) );
    vec3  ro = ta + 1.85*vec3( cos(7.0*mo.x), 0.25, sin(7.0*mo.x) );
    float fl = 2.5;

    if( time>9.0 && time<16.0) { ta.z += 0.1; fl = 5.0; }    
    
    // camera-to-world transformation and ray direction
    mat3 ca = setCamera( ro, ta, 0.05 );
    vec3 rd = normalize( ca*vec3(p,fl) );
    
    // background
    vec3 back = vec3(0.0003,0.013,0.04)*(1.0-clamp(-1.25*rd.y,0.0,1.0));
    
    // render creature
    float ft = -1.0;
    vec3 col = back;
    
    // bounding sphere
    vec2 b = sphIntersect( ro, rd, 1.2 );
    if( b.y>0.0 )
    {
        // raymarch creature from bounding sphere's entry to exit point
        vec4  sum = vec4(0.0);
        float tmax = b.y;
        float t = max(0.0,b.x);
        for( int i=0; i<256 && t<tmax; i++ )
        {
            vec4  res = map( ro + t*rd, time );
            float dis = res.x;
            
            // raymarching step size (outter vs inner)
            float dt = (dis>0.0) ? dis*0.8+0.001 : (-dis+0.002);
            
            // inside creature
            if( dis<0.0 )
            {
                // record depth-buffer
                if( ft<0.0 ) ft=t; 
                
                // local geometry (position, normal, convexity)
                vec3  pos = ro + t*rd;
                vec3  nor = calcNormal( pos, res.x, time );
                float occ = calcAO( pos, nor, time );

                // color and opacity
                vec4 tmp = vec4(res.yzw*res.yzw,min(20.0*dt,1.0));

                // main light
                float ll = 15.0*exp2(-2.0*t);
                tmp.rgb *= (0.5+0.5*dot(nor,-rd))*ll*3.0/(1.0+ll);
                
                // subsurface scattering
                float fre = clamp(1.0+dot(nor,rd),0.0,1.0);
                tmp.rgb += fre*fre*(0.5+0.5*tmp.rgb)*0.8;
                
                // occlusion
                tmp.rgb *= 1.6*mix(tmp.rgb*0.1+vec3(0.2,0.0,0.0),vec3(1.0),occ*1.4);
      
                // fog
                //tmp.rgb = mix( back, tmp.rgb, exp2(-0.1*t*vec3(4.0,3.5,3.0)/fl) );

                // composite front to back, and exit if opaque
                tmp.rgb *= tmp.a;
                sum += tmp*(1.0-sum.a);
                if( sum.a>0.995 ) break;
            }
            t += dt;
        }

        // composite with background
        sum = clamp(sum,0.0,1.0);
        col = col*(1.0-sum.w) + sum.xyz;
    }
    
    // render cita
    vec4  cita = raycastCITA( ro, rd, 2.0/fl, (ft>0.0) ? ft : 15.0, time );
    if( cita.x>0.0 )
    {
        // color
        vec3 citacol = vec3(0.9,1.0,1.0);
        // fog
        citacol = mix( back, citacol, exp2(-0.1*cita.x*vec3(4.0,3.5,3.0)/fl) );
        // blend in
        float fre = clamp(dot(normalize(cita.yzw),rd),0.0,1.0);
        col = mix( col, citacol, fre*0.3 );
    }

    // gain
    //col = col*3.0/(3.0+col);
    
    // gamma and color tweak
    col = pow( col, vec3(0.45,0.5,0.5) );
    
    // vignette
    col *= 1.2 - 0.35*length(p);
    
    // dither to remove banding in the background
    col += fract(sin(fragCoord.x*vec3(13,1,11)+fragCoord.y*vec3(1,7,5))*158.391832)/255.0;
 
    // return color
    fragColor = vec4(col, 1);
}
  ''',
  },
  {
    // https://www.shadertoy.com/view/tsXBzS
    'url': 'https://www.shadertoy.com/view/tsXBzS',
    'fragment': '''
vec3 palette(float d){
	return mix(vec3(0.2,0.7,0.9),vec3(1.,0.,1.),d);
}

vec2 rotate(vec2 p,float a){
	float c = cos(a);
    float s = sin(a);
    return p*mat2(c,s,-s,c);
}

float map(vec3 p){
    for( int i = 0; i<8; ++i){
        float t = iTime*0.2;
        p.xz =rotate(p.xz,t);
        p.xy =rotate(p.xy,t*1.89);
        p.xz = abs(p.xz);
        p.xz-=.5;
	}
	return dot(sign(p),p)/5.;
}

vec4 rm (vec3 ro, vec3 rd){
    float t = 0.;
    vec3 col = vec3(0.);
    float d;
    for(float i =0.; i<64.; i++){
		vec3 p = ro + rd*t;
        d = map(p)*.5;
        if(d<0.02){
            break;
        }
        if(d>100.){
        	break;
        }
        //col+=vec3(0.6,0.8,0.8)/(400.*(d));
        col+=palette(length(p)*.1)/(400.*(d));
        t+=d;
    }
    return vec4(col,1./(d*100.));
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord-(iResolution.xy/2.))/iResolution.x;
	vec3 ro = vec3(0.,0.,-50.);
    ro.xz = rotate(ro.xz,iTime);
    vec3 cf = normalize(-ro);
    vec3 cs = normalize(cross(cf,vec3(0.,1.,0.)));
    vec3 cu = normalize(cross(cf,cs));
    
    vec3 uuv = ro+cf*3. + uv.x*cs + uv.y*cu;
    
    vec3 rd = normalize(uuv-ro);
    
    vec4 col = rm(ro,rd);
    
    
    fragColor = col;
}

/** SHADERDATA
{
	"title": "fractal pyramid",
	"description": "",
	"model": "car"
}
*/
  ''',
  },
  {
    // https://www.shadertoy.com/view/fssXzN
    'url': 'https://www.shadertoy.com/view/fssXzN',
    'fragment': '''
//by neozhaoliang
//https://www.shadertoy.com/user/neozhaoliang

#define AA                 1//2
#define FOV_DIST           3.0
#define BULB_ITERATIONS    7//8
#define SPONGE_ITERATIONS  9
#define MAX_TRACE_STEPS    200//200
#define MIN_TRACE_DIST     0.01
#define MAX_TRACE_DIST     10.0
#define PRECISION          1e-4
#define PI                 3.14159265358979323
#define T                  (iTime * 0.1)

// view to world transformation
mat3 viewMatrix(vec3 camera, vec3 lookat, vec3 up)
{
    vec3 f = normalize(lookat - camera);
    vec3 r = normalize(cross(f, up));
    vec3 u = normalize(cross(r, f));
    return mat3(r, u, -f);
}

// 2D rotatation
void R(inout vec2 p, float a)
{
    p = cos(a) * p + sin(a) * vec2(p.y, -p.x);
}

float mandelbulb(vec3 p)
{
    p /= 1.192;
    p.xyz = p.xzy;
    vec3 z = p;
    vec3 dz = vec3(0.0);
    float dr = 1.0;
    float power = 8.0;
    float r, theta, phi;
    for (int i = 0; i < BULB_ITERATIONS; i++)
    {
        r = length(z);
        if (r > 2.0)
            break;
        float theta = atan(z.y / z.x);
        float phi = asin(z.z / r);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        r = pow(r, power);
        theta = theta * power;
        phi = phi * power;
        z = r * vec3(cos(theta) * cos(phi), cos(phi) * sin(theta), sin(phi)) + p;
    }
    return 0.5 * log(r) * r / dr;
}

float sdSponge(vec3 z)
{
    for(int i = 0; i < SPONGE_ITERATIONS; i++)
    {
        z = abs(z);
        z.xy = (z.x < z.y) ? z.yx : z.xy;
        z.xz = (z.x < z.z) ? z.zx : z.xz;
        z.zy = (z.y < z.z) ? z.yz : z.zy;	 
        z = z * 3.0 - 2.0;
        z.z += (z.z < -1.0) ? 2.0 : 0.0;
    }
    z = abs(z) - vec3(1.0);
    float dis = min(max(z.x, max(z.y, z.z)), 0.0) + length(max(z, 0.0)); 
    return dis * 0.6 * pow(3.0, -float(SPONGE_ITERATIONS)); 
}

float DE(vec3 p)
{
    float d1 = mandelbulb(p);
    float d2 = sdSponge(p);
    return max(d1, d2);
    
}

vec3 calcNormal(vec3 p)
{
    const vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
			  DE(p + e.xyy) - DE(p - e.xyy),
			  DE(p + e.yxy) - DE(p - e.yxy),
			  DE(p + e.yyx) - DE(p - e.yyx)));
}

float trace(vec3 ro, vec3 rd)
{
    float t = MIN_TRACE_DIST;
    float h;
    for (int i = 0; i < MAX_TRACE_STEPS; i++)
    {
        h = DE(ro + rd * t);
        if (h < PRECISION * t || t > MAX_TRACE_DIST)
            return t;
        t += h;
    }
    return -1.0;
}

float softShadow(vec3 ro, vec3 rd, float tmin, float tmax, float k)
{
    float res = 1.0;
    float t = tmin;
    for (int i = 0; i < 10; i++)
    {
        float h = DE(ro + rd * t);
        res = min(res, k * h / t);
        t += clamp(h, 0.01, 0.1);
        if (h < 0.001 || t > tmax)
            break;
    }
    return clamp(res, 0.0, 1.0);
}

float calcAO(vec3 p, vec3 n)
{
    float occ = 0.0;
    float sca = 1.0;
    for (int i = 0; i < 5; i++)
    {
        float h = 0.01 + 0.15 * float(i) / 4.0;
        float d = DE(p + h * n);
        occ += (h - d) * sca;
        sca *= 0.9;
    }
    return clamp(1.0 - 3.0 * occ, 0.0, 1.0);
}

vec3 render(vec3 ro, vec3 rd, vec3 lig)
{
    vec3 background = vec3(0.08, 0.16, 0.32);
    vec3 col = background;
    float t = trace(ro, rd);
    if (t >= 0.0)
	{
        col = vec3(0.9);
        vec3 pos = ro + rd * t;
        vec3 nor = calcNormal(pos);
        vec3 ref = reflect(rd, nor);

        float occ = calcAO(pos, nor);
        float amb = 0.3;
        float dif = clamp(dot(nor, lig), 0.0, 1.0);
        float fre = pow(clamp(1.0 + dot(nor, rd), 0.0, 1.0), 2.0);
        float spe = pow(clamp(dot(ref, lig), 0.0, 1.0), 16.0);
        dif *= softShadow(pos, lig, 0.02, 5.0, 16.0);

        vec3 lin = vec3(0.3);
        lin += 1.8 * dif * vec3(1.0, 0.8, 0.55);
        lin += 2.0 * spe * vec3(1.0, 0.9, 0.7) * dif;
        lin += 0.3 * amb * vec3(0.4, 0.6, 1.0) * occ;
        lin += 0.25 * fre * vec3(1.0) * occ;

        col *= lin;

        float atten = 1.0 / (1.0 + t * t * 0.1);
	    col *= atten * occ;
	    col = mix(col, background, smoothstep(0.0, 0.95, t / MAX_TRACE_DIST));
    }
    return sqrt(col);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

    vec3 tot = vec3(0.0);
    for (int ii = 0; ii < AA; ii ++)
	{
	    for (int jj = 0; jj < AA; jj++)
		{
		    // map uv to (-1, 1) and adjust aspect ratio
		    vec2 offset = vec2(float(ii), float(jj)) / float(AA);
		    vec2 uv = (fragCoord.xy + offset) / iResolution.xy;
		    uv = 2.0 * uv - 1.0;
		    uv.x *= iResolution.x / iResolution.y;
		    vec3 camera = vec3(4.0) / (min(3.5, 1.0 + pow(2.0, T)));
		    vec3 lookat = vec3(0.0);
		    vec3 up = vec3(0.0, 1.0, 0.0);
		    // set camera
		    vec3 ro = camera;
            R(ro.xz, T);
		    mat3 M = viewMatrix(ro, lookat, up);
		    // put screen at distance FOV_DISt in front of the camera
		    vec3 rd = M * normalize(vec3(uv, -FOV_DIST));
            vec3 lig = normalize(vec3(1.0, 2.0, 1.0));
		    vec3 col = render(ro, rd, lig);
		    tot += col;
		}
	}
    tot /= float(AA * AA);
    fragColor = vec4(tot, 1.0);
}
  ''',
  },
  {
    // https://www.shadertoy.com/view/4ds3zn
    'url': 'https://www.shadertoy.com/view/4ds3zn',
    'fragment': '''
// Created by inigo quilez - iq/2013
//   https://www.youtube.com/c/InigoQuilez
//   https://iquilezles.org/
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.


// I can't recall where I learnt about this fractal.
//
// Coloring and fake occlusions are done by orbit trapping, as usual.


// Antialiasing level
#if HW_PERFORMANCE==0
#define AA 1
#else
#define AA 2 // Make it 3 if you have a fast machine
#endif

vec4 orb; 

float map( vec3 p, float s )
{
	float scale = 1.0;

	orb = vec4(1000.0); 
	
	for( int i=0; i<8;i++ )
	{
		p = -1.0 + 2.0*fract(0.5*p+0.5);

		float r2 = dot(p,p);
		
        orb = min( orb, vec4(abs(p),r2) );
		
		float k = s/r2;
		p     *= k;
		scale *= k;
	}
	
	return 0.25*abs(p.y)/scale;
}

float trace( in vec3 ro, in vec3 rd, float s )
{
	float maxd = 30.0;
    float t = 0.01;
    for( int i=0; i<512; i++ )
    {
	    float precis = 0.001 * t;
        
	    float h = map( ro+rd*t, s );
        if( h<precis||t>maxd ) break;
        t += h;
    }

    if( t>maxd ) t=-1.0;
    return t;
}

vec3 calcNormal( in vec3 pos, in float t, in float s )
{
    float precis = 0.001 * t;

    vec2 e = vec2(1.0,-1.0)*precis;
    return normalize( e.xyy*map( pos + e.xyy, s ) + 
					  e.yyx*map( pos + e.yyx, s ) + 
					  e.yxy*map( pos + e.yxy, s ) + 
                      e.xxx*map( pos + e.xxx, s ) );
}

vec3 render( in vec3 ro, in vec3 rd, in float anim )
{
    // trace	
    vec3 col = vec3(0.0);
    float t = trace( ro, rd, anim );
    if( t>0.0 )
    {
        vec4 tra = orb;
        vec3 pos = ro + t*rd;
        vec3 nor = calcNormal( pos, t, anim );

        // lighting
        vec3  light1 = vec3(  0.577, 0.577, -0.577 );
        vec3  light2 = vec3( -0.707, 0.000,  0.707 );
        float key = clamp( dot( light1, nor ), 0.0, 1.0 );
        float bac = clamp( 0.2 + 0.8*dot( light2, nor ), 0.0, 1.0 );
        float amb = (0.7+0.3*nor.y);
        float ao = pow( clamp(tra.w*2.0,0.0,1.0), 1.2 );

        vec3 brdf  = 1.0*vec3(0.40,0.40,0.40)*amb*ao;
        brdf += 1.0*vec3(1.00,1.00,1.00)*key*ao;
        brdf += 1.0*vec3(0.40,0.40,0.40)*bac*ao;

        // material		
        vec3 rgb = vec3(1.0);
        rgb = mix( rgb, vec3(1.0,0.80,0.2), clamp(6.0*tra.y,0.0,1.0) );
        rgb = mix( rgb, vec3(1.0,0.55,0.0), pow(clamp(1.0-2.0*tra.z,0.0,1.0),8.0) );

        // color
        col = rgb*brdf*exp(-0.2*t);
    }

    return sqrt(col);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float time = iTime*0.25;
    float anim = 1.1 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*iTime) );
    
    vec3 tot = vec3(0.0);
    #if AA>1
    for( int jj=0; jj<AA; jj++ )
    for( int ii=0; ii<AA; ii++ )
    #else
    int ii = 1, jj = 1;
    #endif
    {
        vec2 q = fragCoord.xy+vec2(float(ii),float(jj))/float(AA);
        vec2 p = (2.0*q-iResolution.xy)/iResolution.y;

        // camera
        vec3 ro = vec3( 2.8*cos(0.1+.33*time), 0.4 + 0.30*cos(0.37*time), 2.8*cos(0.5+0.35*time) );
        vec3 ta = vec3( 1.9*cos(1.2+.41*time), 0.4 + 0.10*cos(0.27*time), 1.9*cos(2.0+0.38*time) );
        float roll = 0.2*cos(0.1*time);
        vec3 cw = normalize(ta-ro);
        vec3 cp = vec3(sin(roll), cos(roll),0.0);
        vec3 cu = normalize(cross(cw,cp));
        vec3 cv = normalize(cross(cu,cw));
        vec3 rd = normalize( p.x*cu + p.y*cv + 2.0*cw );

        tot += render( ro, rd, anim );
    }
    
    tot = tot/float(AA*AA);
    
	fragColor = vec4( tot, 1.0 );	

}
'''
  },
  {
    // https://www.shadertoy.com/view/7stGRj
    'url': 'https://www.shadertoy.com/view/7stGRj',
    'fragment': '''
// 'Dune (Sand Worm)' dean_the_coder (Twitter: @deanthecoder)
// https://www.shadertoy.com/view/7stGRj (YouTube: https://youtu.be/GqkO68U4Hws)
//
// Processed by 'GLSL Shader Shrinker'
// (https://github.com/deanthecoder/GLSLShaderShrinker)
//
// I'm kinda excited about the new Dune movie, but hope they
// don't mess it up. The 1984 version was the one for me!
//
// Thanks to Evvvvil, Flopine, Nusan, BigWings, Iq, Shane,
// totetmatt, Blackle, Dave Hoskins, byt3_m3chanic, and a bunch
// of others for sharing their time and knowledge!

// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

#define R	iResolution
#define NM	normalize
#define Z0	min(iTime, 0.)
#define sat(x)	clamp(x, 0., 1.)
#define S01(a)	smoothstep(0., 1., a)
#define S(a, b, c)	smoothstep(a, b, c)
#define minH(a, b, c) { float h_ = a; if (h_ < h.d) h = Hit(h_, b, c); }

float t;
struct Hit {
	float d;
	int id;
	vec3 uv;
};

float n31(vec3 p) {
	const vec3 s = vec3(7, 157, 113);
	vec3 ip = floor(p);
	p = fract(p);
	p = p * p * (3. - 2. * p);
	vec4 h = vec4(0, s.yz, s.y + s.z) + dot(ip, s);
	h = mix(fract(sin(h) * 43758.545), fract(sin(h + s.x) * 43758.545), p.x);
	h.xy = mix(h.xz, h.yw, p.y);
	return mix(h.x, h.y, p.z);
}

float n21(vec2 p) { return n31(vec3(p, 1)); }

float smin(float a, float b, float k) {
	float h = sat(.5 + .5 * (b - a) / k);
	return mix(b, a, h) - k * h * (1. - h);
}

float box(vec3 p, vec3 b) { return length(max(abs(p) - b, 0.)); }

float cap(vec3 p, vec2 h) {
	p.y -= clamp(p.y, 0., h.x);
	return length(p) - h.y;
}

Hit map(vec3 p) {
	float d, e, g, lp, r, rz,
	      f = S(0., 5., t),
	      n = n31(p * 4.);
	d = n21(p.xz * .1) * 3. + p.y + 2.5;
	g = smin(d, length(p - vec3(.2, -8.6, 12.6)) - 6. + .01 * (.5 + .5 * sin(p.y * 22.)), 1.);
	p += vec3(.5 + sin(t * .6) * .2 + .6 * sin(p.z * .4 - .66), 1. - cos(p.z * .3 - .3 - f * mix(.8, 1., S01(sin(t * 1.4) * .5 + .5))) * 1.8, S(28., 30., t) * 2.5 - mix(6., 2.8, f));
	r = .8 + smin(p.z * .18, 2., .5) + abs(sin(p.z * 2.) * S01(p.z) * .05);
	r *= S(-5.3 + 2.75 * cos(t * .8) * f, 1.4, p.z);
	lp = length(p.xy);
	f = abs(lp - r - .05) - .03;
	r *= S(2.5, .35 + sin(t) * .1, p.z);
	d = max(abs(lp - r) - .02, .4 - p.z);
	p.xy = vec2(fract(atan(p.y, p.x) * .477) - .5, lp);
	p.y -= r;
	Hit h = Hit(min(d, box(p, vec3(.2 + p.z * .77, .02, .4))), 2, p);
	p.y += .13;
	vec2 v2 = vec2(.1, sat(.07 * p.y));
	p.z -= .4;
	rz = mod(p.z, .3) - .15;
	e = max(min(cap(vec3(mod(p.x, .08333) - .04167, p.y, rz), v2), cap(vec3(mod(p.x + .04167, .08333) - .04167, p.y, rz - .15), v2)), -0.05 - p.z * 0.2);
	d = abs(p.x) - p.z * .5 - .5;
	minH(max(e, d), 4, p);
	f = max(f, d - .05);
	minH(f, 3, p);
	g = smin(g, h.d, .4 + .4 * n * S(1., 0., abs(g - f)));
	minH(g, 1, p);
	return h;
}

vec3 N(vec3 p, float t) {
	float h = t * .4;
	vec3 n = vec3(0);
	for (int i = 0; i < 4; i++) {
		vec3 e = .005773 * (2. * vec3(((i + 3) >> 1) & 1, (i >> 1) & 1, i & 1) - 1.);
		n += e * map(p + e * h).d;
	}

	return NM(n);
}

float shadow(vec3 p, vec3 lp) {
	float d,
	      s = 1.,
	      t = .1,
	      mxt = length(p - lp);
	vec3 ld = NM(lp - p);
	for (float i = Z0; i < 40.; i++) {
		d = map(t * ld + p).d;
		s = min(s, 15. * d / t);
		t += max(.1, d);
		if (mxt - t < .5 || s < .001) break;
	}

	return S01(s);
}

float ao(vec3 p, vec3 n, float h) { return map(h * n + p).d / h; }

float fog(vec3 v) { return exp(dot(v, v) * -.001); }

vec3 lights(vec3 p, vec3 rd, float d, Hit h) {
	vec3 c,
	     ld = NM(vec3(6, 3, -10) - p),
	     n = N(p, d);
	float spe = 1.;
	if (h.id == 3) {
		c = vec3(.4, .35, .3);
		n.y += n31(h.uv * 10.);
		n = NM(n);
	}
	else if (h.id == 2) c = mix(vec3(.16, .08, .07), vec3(.6), pow(n31(h.uv * 10.), 3.));
	else if (h.id == 4) c = vec3(.6, 1, 4);
	else {
		spe = .1;
		c = vec3(.6);
		n.x += sin((p.x + p.z * n.z) * 8.) * .1;
		n = NM(n);
	}

	float ao = mix(ao(p, n, .2), ao(p, n, 2.), .7);
	return mix((sat(.1 + .9 * dot(ld, n)) * (.1 + .9 * shadow(p, vec3(6, 3, -10))) * (.3 + .7 * ao) + (sat(.1 + .9 * dot(ld * vec3(-1, 0, -1), n)) * .3 + pow(sat(dot(rd, reflect(ld, n))), 10.) * spe) * ao) * c * vec3(1.85, .5, .08), vec3(1.85, .5, .08), S(.7, 1., 1. + dot(rd, n)) * .1);
}

vec4 march(inout vec3 p, vec3 rd, float s, float mx) {
	float i,
	      d = .01;
	Hit h;
	for (i = Z0; i < s; i++) {
		h = map(p);
		if (abs(h.d) < .0015) break;
		d += h.d;
		if (d > mx) return vec4(0);
		p += h.d * rd;
	}

	return vec4(lights(p, rd, d, h), h.id);
}

vec3 scene(vec3 rd) {
	t = mod(iTime, 30.);
	vec3 c,
	     p = vec3(0);
	vec4 col = march(p, rd, 180., 64.);
	float f = 1.,
	      x = n31(rd + vec3(-t * 2., -t * .4, t));
	if (col.w == 0.) c = mix(vec3(.5145, .147, .0315), vec3(.22, .06, .01), sat(rd.y * 3.));
	else {
		c = col.rgb;
		f = fog(p * (.7 + .3 * x));
	}

	f *= 1. - x * x * x * .4;
	return mix(vec3(.49, .14, .03), c, sat(f));
}

#define rgba(col)	vec4(pow(max(vec3(0), col), vec3(.45)) * sat(t), 0)

void mainVR(out vec4 fragColor, vec2 fc, vec3 ro, vec3 rd) {
	rd.xz *= mat2(1, 0, 0, -1);
	fragColor = rgba(scene(rd));
}

void mainImage(out vec4 fragColor, vec2 fc) {
	vec2 uv = (fc - .5 * R.xy) / R.y,
	     q = fc.xy / R.xy;
	vec3 r = NM(cross(vec3(0, 1, 0), vec3(0, 0, 1))),
	     col = scene(NM(vec3(0, 0, 1) + r * uv.x + cross(vec3(0, 0, 1), r) * uv.y));
	col *= .5 + .5 * pow(16. * q.x * q.y * (1. - q.x) * (1. - q.y), .4);
	fragColor = rgba(col) * sat(30. - t);
}
'''
  },
  {
    // https://www.shadertoy.com/view/NtlSDs
    'url': 'https://www.shadertoy.com/view/NtlSDs',
    'fragment': '''
// Copyright Inigo Quilez, 2021 - https://iquilezles.org/
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// -------------------------------------------------------

// I made this shader in 2008 for the Euskal Party
// competition in Spain. The original Pouet entry is 
// here: https://www.pouet.net/prod.php?which=51074

// It's a raymarched SDF, and while I had raymarched some
// fractal SDFs back in 2005, this was the first time I
// used the technique to produce art-directed content and
// "paint with maths" (as opposed to doing "mathematical art").

// This is also the shader where I first used the now
// popular ambient occlusion estimation. You'll also find
// my first attemtps at smooth-minumum (used to blend the
// creature's head and tentacles together), and soft 
// shadows (still not as good as the ones we use today).
// You'll find most of the other usual Shadertoy/raymarching
// stuff - domain repetition, Value Noise, box SDFs and of
// course the raymarching machinery.

// Surprisngly, copying the code from my original project
// to Shadertoy here required very little modifications.


#define AA 1

// iFrame not supported here
// #define ZERO min(iFrame,0)
#define ZERO 0

float dot2( in vec3 v ) { return dot(v,v); }

// https://iquilezles.org/articles/sfrand
float sfrand( inout int mirand )
{
    mirand = mirand*0x343fd+0x269ec3;
    float res = uintBitsToFloat((uint(mirand)>>9)|0x40000000u ); 
    return( res-3.0 );
}

// hash by Hugo Elias
uint ihash( uint n )
{
	n = (n << 13) ^ n;
    n = (n*(n*n*15731u+789221u)+1376312589u)&0x7fffffffu;
    return n;
}

// hash by Hugo Elias
float hash( int n )
{
	n = (n << 13) ^ n;
    n = (n*(n*n*15731+789221)+1376312589)&0x7fffffff;
    return 1.0 - float(n)*(1.0/1073741824.0);
}

float noise( vec3 p )
{
	ivec3 ip = ivec3(floor(p));
     vec3 fp = fract(p);

    vec3 w = fp*fp*(3.0-2.0*fp);
    
    int n = ip.x + ip.y*57 + 113*ip.z;

	return mix(mix(mix( hash(n+(0+57*0+113*0)),
                        hash(n+(1+57*0+113*0)),w.x),
                   mix( hash(n+(0+57*1+113*0)),
                        hash(n+(1+57*1+113*0)),w.x),w.y),
               mix(mix( hash(n+(0+57*0+113*1)),
                        hash(n+(1+57*0+113*1)),w.x),
                   mix( hash(n+(0+57*1+113*1)),
                        hash(n+(1+57*1+113*1)),w.x),w.y),w.z);
}

// https://iquilezles.org/articles/fbm
float fbm( vec3 p )
{
#if 0
    // original code
    return 0.5000*noise( p*1.0 ) + 
           0.2500*noise( p*2.0 ) + 
           0.1250*noise( p*4.0 ) +
           0.0625*noise( p*8.0 );
#else
    // equivalent code, but compiles MUCH faster
    float f = 0.0;
    float s = 0.5;
    for( int i=ZERO; i<4; i++ )
    {
        f += s*noise( p );
        s *= 0.5;
        p *= 2.0;
    }
    return f;
#endif
}

// https://iquilezles.org/articles/distfunctions
float sdBox( in vec3 p, in vec3 b ) 
{
    vec3 q = abs(p) - b;
    return min(max(q.x,max(q.y,q.z)),0.0) + length(max(q,0.0));
}

// https://iquilezles.org/articles/distfunctions
float udSqBox( vec3 p, vec3 b )
{
	float di = 0.0;
	float dx = abs(p.x)-b.x; if( dx>0.0 ) di+=dx*dx;
    float dy = abs(p.y)-b.y; if( dy>0.0 ) di+=dy*dy;
    float dz = abs(p.z)-b.z; if( dz>0.0 ) di+=dz*dz;
    return di;
}

//-------------------------------------------------------------------------------

float columna( vec3 pos, float offx )
{
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    
	float y2=y-0.40;
    float y3=y-0.35;
    float y4=y-1.00;

    float di =    udSqBox( vec3(x, y , z), vec3(0.10, 1.00, 0.10) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.12, 0.40, 0.12) ) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.05, 0.35, 0.14) ) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.14, 0.35, 0.05) ) );
    di = min( di, udSqBox( vec3(x, y4, z), vec3(0.14, 0.02, 0.14) ) );
    di = min( di, udSqBox( vec3((x-y2)*0.7071, (y2+x)*0.7071, z), vec3(0.10*0.7071,  0.10*0.7071, 0.12) ) );
    di = min( di, udSqBox( vec3(x, (y2+z)*0.7071, (z-y2)*0.7071), vec3(0.12,  0.10*0.7071, 0.1*0.7071) ) );
    di = min( di, udSqBox( vec3((x-y3)*0.7071, (y3+x)*0.7071, z), vec3(0.10*0.7071,  0.10*0.7071, 0.14) ) );
    di = min( di, udSqBox( vec3(x, (y3+z)*0.7071, (z-y3)*0.7071), vec3(0.14,  0.10*0.7071, 0.10*0.7071) ) );

    #if 1
    float fb = fbm(vec3(10.1*x+offx,10.1*y,10.1*z));
    if( fb>0.0 )
        di = di + 0.00000003*fb;
    #endif

    return sqrt(di);
}

float bicho( vec3 pos, float mindist )
{
    pos -= vec3(0.64,0.50,1.5);
    
    float r2 = dot2(pos);

	float sa = smoothstep(0.0,0.5,r2);
    pos *= vec3(0.75,0.80,0.75) + sa*vec3(0.25,0.20,0.25);
    r2 = dot2(pos);

    if( r2>5.0 ) return mindist;
    if( pos.y >0.5 ) return mindist;
	if( pos.y>-0.2 && dot(pos.xz,pos.xz)>0.6 ) return mindist;
    if( r2>(1.7+mindist)*(1.7+mindist)  ) return mindist;	// algo ayuda, poco, pero algo oiga

	float r = sqrt(r2);

	if( r<0.75 )
    {
	float a1 = 1.0-smoothstep( 0.0, 0.75, r );
	a1 *= 0.6;
	vec2 sico = vec2( sin(a1), cos(a1) );
	float nx = pos.x;
	float ny = pos.y;
	pos.x = nx*sico.y - ny*sico.x;
	pos.y = nx*sico.x + ny*sico.y;
    }
	#define TENTACURA 0.045
	float mindist2 = 100000.0;
    vec3  p = pos;
	float rr = 0.05+length(pos.xz);
	float ca = (0.5-TENTACURA*0.75) -6.0*rr*exp2(-10.0*rr);
    for( int j=1+ZERO; j<7; j++ )
	{
		float an = (6.2831/7.0) * float(j);
		float aa = an + 0.40*rr*noise(vec3(4.0*rr, 2.5, an)) + 0.29;
		float rc = cos(aa);
        float rs = sin(aa);
		vec3 q = vec3( p.x*rc-p.z*rs, p.y+ca, p.x*rs+p.z*rc );
		float dd = dot(q.yz,q.yz);
		if( q.x>0.0 && q.x<1.5 && dd<mindist2 ) mindist2=dd;
	}
	float dtentacles = sqrt(mindist2) - TENTACURA;

    // proto-smooth-minimum
	float dhead = r-0.15;

    float h = clamp( r*3.0, 0.0, 1.0 );
    return mix(dhead-0.15,dtentacles,h);
}

float techo2( float x, float y, float z )
{
    y = 1.0 - y;
    x = x - 0.5;
    z = z - 0.5;
    return 0.4 - sqrt(y*y+min(x*x,z*z) );
}

float map( vec3 pos, out uint sid, out uint submat )
{
    submat = 0u;
    float mindist;
    
    //-----------------------
    // suelo
    //-----------------------
    {
	float dis = pos.y;
	
	float ax = 128.0 + (pos.x+pos.z)*6.0;
	float az = 128.0 + (pos.x-pos.z)*6.0;
    uint ix = uint(floor(ax));
    uint iz = uint(floor(az));
    submat = ihash(ix+53u*iz);
    bool ba = ( ((submat>>10)&7u)>6u );
    float peld = max(fract(ax),fract(az));
    peld = smoothstep( 0.975, 1.0, peld );
    if( ba ) peld = 1.0;
    dis += 0.005*peld;
	mindist = dis;
    if( peld>0.0000001 ) sid = 2u; else sid = 0u;
    }
    
    //-----------------------
    // techo
    //-----------------------
	if( pos.y>1.0 ) 
    {
        float dis = techo2( fract(pos.x), pos.y, fract(pos.z) );
        if( dis<mindist )
        {
            mindist = dis;
            sid = 5u;
        }
    }

    //-----------------------
    // columnas
    //-----------------------
    {
	vec3 fc = vec3(fract( pos.x+0.5 )-0.5, pos.y,
	               fract( pos.z+0.5 )-0.5);
    float bb = udSqBox( fc, vec3(0.14, 1.0, 0.14) );
    if( bb < mindist*mindist )
    {
        float dis = columna( fc, 13.1*floor(pos.x)+17.7*floor(pos.z) );
        if( dis<mindist )
        {
            mindist = dis;
            sid = 1u;
        }
    }
    }
    
    //-----------------------
    // bicho
    //-----------------------
    //if( length( pos-vec3(0.64,-2.1,1.5) ) - 2.8 < mindist )
    if( dot2( pos-vec3(0.64,-2.1,1.5) ) < (mindist+2.8)*(mindist+2.8) )
    {
    float dis = bicho( pos, mindist );
    if( dis<mindist )
    {   
        mindist = dis;
        sid = 4u;
    }
    }

    //-----------------------
    // baldosas
    //-----------------------
    if( pos.y-0.02 < mindist )
	{
	int kkr = 65535;
    for( int i=ZERO; i<10; i++ )
    {
        float a = 3.14*sfrand(kkr);
        float co = cos(a);
        float si = sin(a);
        float xc = pos.x - (0.5 + 1.5*sfrand(kkr));
        float zc = pos.z - (1.0 + 1.0*sfrand(kkr));
        float xo = xc*co-si*zc;
        float zo = zc*co+si*xc;
        float dis = sdBox( vec3(xo, pos.y-0.008, zo), vec3(0.07, 0.008, 0.07) );
        if( dis < mindist ) 
        {
            mindist = dis;
            sid = 1u;
        }
    }
	}
    
    return mindist;
}

void raycast( vec3 ro, vec3 rd, float to, float tMax, out float resT, out vec3 resPos, out uint resSurface, out uint submat )
{
    float t = to;
    for( int i=ZERO; i<256; i++ )
	{
        resPos = ro + t*rd;
		float h = map( resPos, resSurface, submat );
		if( h<0.0001 )
		{
		    resT = t;
			return;
		}
        t += h*0.75;
        if( t>tMax ) break;
	}
    resSurface = 666u;
}

vec3 calcNormal( vec3 pos )
{
#if 0
    // original code
    const float eps = 0.0002;
    uint kk, kk2;
    vec3 nor = vec3(
      map( pos.x+eps, pos.y, pos.z, kk, kk2 ) - map( pos.x-eps, pos.y, pos.z, kk, kk2 ),
      map( pos.x, pos.y+eps, pos.z, kk, kk2 ) - map( pos.x, pos.y-eps, pos.z, kk, kk2 ),
      map( pos.x, pos.y, pos.z+eps, kk, kk2 ) - map( pos.x, pos.y, pos.z-eps, kk, kk2 ));
    return normalize(nor);
#else
    // inspired by tdhooper and klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        uint kk, kk2;
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*map(pos+0.0002*e, kk, kk2);
        if( n.x+n.y+n.z>100.0 ) break;
    }
    return normalize(n);
#endif    
}

vec3 shade( vec3 pos, vec3 rd, uint matID, uint subMatID, float dis )
{
    if( matID==666u ) return vec3(0.0,0.0,0.0);

    vec3 col = vec3(0.1*dis);

    // calc normal
    vec3 nor = calcNormal( pos );

    // bump mapping
    #if 1
    const float kke = 0.0001;
    float bumpa = 0.0075;
	if( matID!=5u ) bumpa *= 0.75;
    if( matID==4u ) bumpa *= 0.50;
    bumpa /= kke;
    float kk = fbm( 32.0*pos );
    nor.x += bumpa*(fbm( 32.0*vec3(pos.x+kke, pos.y, pos.z) )-kk);
    nor.y += bumpa*(fbm( 32.0*vec3(pos.x, pos.y+kke, pos.z) )-kk);
    nor.z += bumpa*(fbm( 32.0*vec3(pos.x, pos.y, pos.z+kke) )-kk);
    nor = normalize(nor);
    #endif


    // light
	float spe = 0.0;
    vec3 lig = vec3( 0.5, 0.8, 1.5 ) - pos;
    float llig = dot2(lig);
    float im = 1.0/sqrt(llig);
    lig *= im;
    float dif = dot( nor, lig ); 
    if( matID==4u )
        dif = 0.5+0.5*dif;
    else
        dif = 0.10+0.90*dif;
    dif = max( dif, 0.0 );
	dif *= 2.5*exp2( -1.75*llig );
	float dif2 = (nor.x+nor.y)*0.075;

    // materials
    if( matID==0u )
    {
        float xoff = 13.1 * float(subMatID&255u);
        float fb = fbm( 16.0*vec3(pos.x+xoff, pos.y, pos.z) );

        col = 0.7 + vec3(0.20,0.22,0.25)*fb;

        float baldscale = float((subMatID>>9)&15u)/14.0;
        baldscale = 0.51 + 0.34*baldscale;
        col *= baldscale;
        float fx = 1.0;
        if( (subMatID&256u)!=0u ) fx = -1.0;
        float m = sin( 64.0*pos.z*fx +  64.0*pos.x + 4.0*fb );
        m = smoothstep( 0.25, 0.5, m ) - smoothstep( 0.5, 0.75, m );

        col += m*0.15;
    }
    else if( matID==2u )
    {
        col = vec3(0.0,0.0,0.0);
    }
    else if( matID==1u )
    {
        float fb = fbm( 16.0*pos );
        float m = sin( 64.0*pos.z +  64.0*pos.x + 4.0*fb );
        m = smoothstep( 0.3, 0.5, m ) - smoothstep( 0.5, 0.7, m );
        col = 0.59 + fb*vec3(0.17,0.18,0.21) + m*0.15 + dif2;
    }   
    else if( matID==4u )
    {
        float ft = fbm( 16.0*pos );
        col = vec3(0.82,0.73,0.65) + ft*vec3(0.10,0.10,0.15);
        float fs = 0.90+0.10*fbm( 32.0*pos );
        col *= fs;

        float fre = -dot( nor, rd );
        if( fre<0.0 ) fre=0.0;
        fre = fre*fre*0.45;
        col -= fre;

		spe = (nor.y-nor.z)*0.707;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe *= 0.20;
     }   
    // techo
    else //if( matID==5 )
    {
        float fb = fbm( 16.0*pos );
        col = vec3(0.64,0.61,0.59) + fb*vec3(0.21,0.19,0.19) + dif2;
    }   

    // AO
    float ao;
    #if 1
    float totao = 0.0;
    float sca = 10.0;
    for( int aoi=ZERO; aoi<5; aoi++ )
    {
        float hr = 0.01 + 0.015*float(aoi*aoi);
        vec3 aopos = pos + hr*nor;
        uint kk, kk2;
        float dd = map( aopos, kk, kk2 );
        ao = -(dd-hr);
        totao += ao*sca;
        sca *= 0.5;
        //if( totao>1000.0+sin(iTime) ) break;
    }
    ao = 1.0 - clamp( totao, 0.0, 1.0 );
    #else
    ao = 1.0;
    #endif

    // shadow
    #if 1
    float so = 0.0;
	for( int i=ZERO; i<6; i++ )
    {
		float h = float(i)/6.0;
        float hr = 0.01 + 1.0*h;
        vec3 aopos = pos + hr*lig;
        uint kk, kk2;
        float dd = map( aopos, kk, kk2 );
        so += (1.0-h)*dd*20.0/6.0;
        //if( so>1000.0+iTime ) break;
    }
    dif *= clamp( (so-0.40)*1.5, 0.0, 1.0 );
    #endif

    // shade
	float ao2 = 0.05 + 0.95*ao;
    col *= vec3(0.25,0.30,0.35)*ao + dif*ao2*vec3(1.95,1.65,1.05);
    col += spe;
    col *= exp2( -0.40*pos.z );

    return col;
}

vec3 post( vec3 col, vec2 p )
{
    // flare
	float rr = (p.x+0.10)*(p.x+0.10) + (p.y-0.40)*(p.y-0.40);
    col += vec3( 1.0, 0.95, 0.90 )*0.75*exp2( -64.0*rr );

    // color correct
    col = sqrt(col)*0.3 + 0.7*col;
    col *= vec3(0.83,1.00,0.83);

    // vignetting
    col *= 0.25 + 0.75*clamp( 0.60*abs(p.x-1.77)*abs(p.x+1.77), 0.0, 1.0 );
    
    return clamp( col, 0.0, 1.0 );
}

vec3 computePixel( vec2 p, vec2 res, float time )
{
    vec2 s = (2.0*p-res)/res.y;
    
    // fisheye distortion
    float r2 = s.x*s.x*0.32 + s.y*s.y;
	float tt = (7.0-sqrt(37.5-11.5*r2))/(r2+1.0);

    // create camera and ray
    vec2 d = s*tt;
#if 0
    vec3 ro = vec3( 0.195, 0.5, 0.0 );
	vec3 rd = normalize( vec3( d.x*0.955336 + 0.29552, d.y, 0.955336 - d.x*0.29552 ) );
#else
    float an = 4.4 + 0.2*sin(0.4*time);
    vec3 ta = vec3(0.64,0.50,1.5);
    vec3 ro = ta + 1.55*vec3( cos(an), 0.0, sin(an));
    vec3 ww = normalize( ta-ro );
    vec3 vv = vec3(0.0,1.0,0.0);
    vec3 uu = normalize(cross(vv,ww));
    vec3 rd = normalize( uu*d.x + vv*d.y + 1.0*ww );
#endif
	float t;
	uint matID, sumMatID;
	vec3 xyz;
    
    // raycast
	raycast( ro, rd, 0.5, 24.0, t, xyz, matID, sumMatID );
    
    // shade
	vec3 col = shade(  xyz, rd, matID, sumMatID, t  );
    
    // postprocess
	col = post( col, s );

	return col;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
#if AA<2    
    vec3 col = computePixel(fragCoord, iResolution.xy, iTime );
#else
    vec3 col = vec3(0.0);
    for( int j=0; j<AA; j++ )
    for( int i=0; i<AA; i++ )
    {
        vec2 of = vec2(i,j)/float(AA);
        float time = iTime + (0.5/30.0)*float(j*AA+i)/float(AA*AA);
        col += computePixel(fragCoord+of, iResolution.xy, time );
    }
    col /= float(AA*AA);
#endif
    fragColor = vec4(col,1.0);
}
'''
  }
];
