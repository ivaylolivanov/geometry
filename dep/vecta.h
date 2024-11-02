#ifndef VECTA_H
#define VECTA_H
#include <cmath>
#include <utility>

#ifdef GSQR
#undef GSQR
#endif
#define GSQR(x) ((x)*(x))

namespace vecta  {
const double PI = 3.14159265358979;

typedef double Number;

template <typename N = Number>
class vec2d  {
private:
  void cp(N xx, N yy)  {x = xx;  y = yy;}
public:
  N x,y;
  vec2d(const N x = 0, const N y = 0)  {cp(x,y);}
  template <class aN>
      vec2d(const vec2d<aN>& q)  {cp(q.x,q.y);}
  template <class aN>
      vec2d<N>& operator= (const vec2d<aN>& q)  {cp(q.x,q.y);  return *this;}
  template <class aN>
      vec2d<N>& operator+= (const vec2d<aN>& q)  {x += q.x;  y += q.y;  return *this;}
  template <class aN>
      vec2d<N>& operator-= (const vec2d<aN>& q)  {x -= q.x;  y -= q.y;  return *this;}
  template <class aN>
      vec2d<N>& operator*= (const aN c)  {x *= c;  y *= c;  return *this;}
  template <class aN>
      vec2d<N>& operator/= (const aN c)  {x /= c;  y /= c;  return *this;}
  template <class aN>
      vec2d<N>& operator&= (const aN a)  {return *this = cos(a)**this+sin(a)*~*this;}
  template <class aN>
      vec2d<N>& operator&= (const vec2d<aN>& q)  {
        N t = x*q.x-y*q.y;
        y = x*q.y+y*q.x;
        x = t;
        return *this;
      }
  template <class aN>
      vec2d<N>& operator/= (const vec2d<aN>& q)  {
        Number t = GSQR(q.x)+GSQR(q.y), xx = q.x/t, yy = q.y/t;
        t = x*xx+y*yy;
        y = y*xx-x*yy;
        x = t;
        return *this;
      }
};

vec2d<> polar(const Number r, const Number a)  {
  return vec2d<>(r*cos(a),r*sin(a));
}

template <typename N>
Number len(const vec2d<N>& p)  {return hypot(p.x,p.y);}

template <typename N>
Number dir(const vec2d<N>& p)  {return atan2(p.y,p.x);}

template <typename N>
vec2d<> unit(const vec2d<N>& p)  {return vec2d<>(p)/len(p);}

template <typename N>
vec2d<N> operator- (const vec2d<N>& p)  {
  return vec2d<N>(-p.x,-p.y);
}

template <typename N>
vec2d<N> operator! (const vec2d<N>& p)  {
  return vec2d<N>(p.x,-p.y);
}

template <typename N>
vec2d<N> operator~ (const vec2d<N>& p)  {
  return vec2d<N>(-p.y,p.x);
}

template <typename N1, typename N2>
vec2d<N1> operator+ (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return vec2d<N1>(p.x+q.x,p.y+q.y);
}

template <typename N1, typename N2>
vec2d<N1> operator- (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return p+-q;
}

template <typename N1, typename N2>
vec2d<N2> operator* (const N1 c, const vec2d<N2>& q)  {
  return vec2d<N2>(c*q.x,c*q.y);
}

template <typename N1, typename N2>
vec2d<N1> operator* (const vec2d<N1>& p, const N2 c)  {
  return c*p;
}

template <typename N1, typename N2>
vec2d<N1> operator/ (const vec2d<N1>& p, const N2 c)  {
  return vec2d<N1>(p.x/c,p.y/c);
}

template <typename N1, typename N2>
N1 operator* (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return static_cast<N1>(p.x*q.x+p.y*q.y);
}

template <typename N1, typename N2>
N1 operator^ (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return ~p*q;
}

template <typename N>
N norm(const vec2d<N>& p)  {return GSQR(p);}

template <typename N1, typename N2>
bool operator== (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return p.x==q.x && p.y==q.y;
}

template <typename N1, typename N2>
bool operator!= (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return p.x!=q.x || p.y!=q.y;
}

template <typename N1, typename N2>
bool operator< (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return (p^q) > 0;
}

template <typename N1, typename N2>
bool operator<= (const vec2d<N1>& p, const vec2d<N2>& q)  {
  auto t = p^q;
  return t > 0 || t == 0 && p*q >= 0;
}

template <typename N1, typename N2>
bool operator|| (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return (p^q) == 0;
}

template <typename N1, typename N2>
bool operator>= (const vec2d<N1>& p, const vec2d<N2>& q)  {
  auto t = p^q;
  return t < 0 || t == 0 && p*q >= 0;
}

template <typename N1, typename N2>
bool operator> (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return (p^q) < 0;
}

template <typename N1, typename N2>
Number angle(const vec2d<N1>& u, const vec2d<N2>& v)  {
  vec2d<> a = unit(u), b = unit(v);
  return atan2(a^b,a*b);
// a faster, more direct, but less robust method: atan2(u^v,u*v)
}

template <typename N1, typename N2>
vec2d<N1> operator& (const vec2d<N1>& p, const N2 & a)  {
  return cos(a)*p+sin(a)*~p;
}

template <typename N1, typename N2>
vec2d<N1> operator& (const vec2d<N1>& p, const vec2d<N2>& q)  {
  return vec2d<N1>(p.x*q.x-p.y*q.y,p.x*q.y+p.y*q.x);
}

template <typename N1, typename N2>
vec2d<N1> operator/ (const vec2d<N1>& p, const vec2d<N2>& q)  {
  Number t = GSQR(q.x)+GSQR(q.y), xx = q.x/t, yy = q.y/t;
  return vec2d<N1>(p.x*xx+p.y*yy,p.y*xx-p.x*yy);
}

//-----------------------------------------------------------------------

template <typename N> class vec3d;

class quatrn  {
private:
  void cp(Number rr, Number xx, Number yy, Number zz)  {r = rr;  x = xx;  y = yy;  z = zz;}
public:
  Number r,x,y,z;
  quatrn(const Number r = 0, const Number x = 0, const Number y = 0, const Number z = 0)  {cp(r,x,y,z);}
  quatrn(const quatrn& q)  {cp(q.r,q.x,q.y,q.z);}
  quatrn& operator= (const quatrn& q)  {cp(q.r,q.x,q.y,q.z);  return *this;}
  quatrn& operator*= (const quatrn& q)  {
    cp(q.r*r-q.x*x-q.y*y-q.z*z,
       q.r*x+q.x*r+q.y*z-q.z*y,
       q.r*y+q.y*r+q.z*x-q.x*z,
       q.r*z+q.z*r+q.x*y-q.y*x);
    return *this;
  }
  template <class aN>
  quatrn(const Number a, const vec3d<aN>& p)  {
    Number d = len(p), s = sin(a/2)/d;
    cp(cos(a/2),s*p.x,s*p.y,s*p.z);
  }
};

quatrn operator* (const quatrn& b, const quatrn& a)  {
  return quatrn(a.r*b.r-a.x*b.x-a.y*b.y-a.z*b.z,
                a.r*b.x+a.x*b.r+a.y*b.z-a.z*b.y,
                a.r*b.y+a.y*b.r+a.z*b.x-a.x*b.z,
                a.r*b.z+a.z*b.r+a.x*b.y-a.y*b.x);
}

template <typename N = Number>
class vec3d  {
private:
  void cp(N xx, N yy, N zz)  {x = xx;  y = yy;  z = zz;}
public:
  N x,y,z;
  vec3d(const N x = 0, const N y = 0, const N z = 0)  {cp(x,y,z);}
  template <class aN>
      vec3d(const vec3d<aN>& q)  {cp(q.x,q.y,q.z);}
  template <class aN>
      vec3d<N>& operator= (const vec3d<aN>& q)  {cp(q.x,q.y,q.z);  return *this;}
  template <class aN>
      vec3d<N>& operator+= (const vec3d<aN>& q)  {x += q.x;  y += q.y;  z += q.z;  return *this;}
  template <class aN>
      vec3d<N>& operator-= (const vec3d<aN>& q)  {x -= q.x;  y -= q.y;  z -= q.z;  return *this;}
  template <class aN>
      vec3d<N>& operator*= (const aN c)  {x *= c;  y *= c;  z *= c;  return *this;}
  template <class aN>
      vec3d<N>& operator/= (const aN c)  {x /= c;  y /= c;  z /= c;  return *this;}
  template <class aN>
      vec3d<N>& operator^= (const vec3d<aN>& q)  {cp(y*q.z-q.y*z,z*q.x-q.z*x,x*q.y-q.x*y);  return *this;}
  vec3d<N>& operator&= (const quatrn& q)  {
    Number p1 = (q.r-q.z)*(q.r+q.z), p2 = (q.x-q.y)*(q.x+q.y),
           ab = q.r*q.x, ac = q.r*q.y, ad = q.r*q.z,
           bc = q.x*q.y, bd = q.x*q.z, cd = q.y*q.z,
           xx = x, yy = y;
    x = (p1+p2)  *xx + 2*(bc-ad)*yy + 2*(bd+ac)         *z,
    y = 2*(bc+ad)*xx + (p1-p2)  *yy + 2*(cd-ab)         *z;
    z = 2*(bd-ac)*xx + 2*(cd+ab)*yy + (4*GSQR(q.r)-2*p1-1)*z;
    return *this;
  }
  template <class aN>
      vec3d<N>& operator&= (const std::pair< aN,vec3d<aN> >& p)  {
        return *this &= quatrn(p.first,p.second);
      }
};

template <typename N>
vec3d<N> operator- (const vec3d<N>& p)  {
  return vec3d<N>(-p.x,-p.y,-p.z);
}

template <typename N1, typename N2>
vec3d<N1> operator+ (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return vec3d<N1>(p.x+q.x,p.y+q.y,p.z+q.z);
}

template <typename N1, typename N2>
vec3d<N1> operator- (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return p+-q;
}

template <typename N1, typename N2>
vec3d<N2> operator* (const N1 c, const vec3d<N2>& q)  {
  return vec3d<N2>(c*q.x,c*q.y,c*q.z);
}

template <typename N1, typename N2>
vec3d<N1> operator* (const vec3d<N1>& p, const N2 c)  {
  return c*p;
}

template <typename N1, typename N2>
vec3d<N1> operator/ (const vec3d<N1>& p, const N2 c)  {
  return vec3d<N1>(p.x/c,p.y/c,p.z/c);
}

template <typename N1, typename N2>
N1 operator* (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return static_cast<N1>(p.x*q.x+p.y*q.y+p.z*q.z);
}

template <typename N1, typename N2>
vec3d<N1> operator^ (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return vec3d<N1>(p.y*q.z-q.y*p.z,p.z*q.x-q.z*p.x,p.x*q.y-q.x*p.y);
}

template <typename N>
std::pair < vec3d<N>, vec3d<N> > operator~ (const vec3d<N>& p)  {
  vec3d<N> a;
  N x = fabs(p.x), y = fabs(p.y), z = fabs(p.z);
  int bx = x<=y && x<=z, by = y<=z && y<=x, bz = z<=x && z<=y;
  if       (by<bx)  a = vec3d<N>(0,-p.z,p.y);
  else  if (bz<by)  a = vec3d<N>(p.z,0,-p.x);
  else              a = vec3d<N>(-p.y,p.x,0);
  return std::make_pair(a,p^a);
}

template <typename N>
N norm(const vec3d<N>& p)  {return GSQR(p);}

template <typename N>
Number len(const vec3d<N>& p)  {return sqrt(GSQR(p));}

template <typename N>
vec3d<> unit(const vec3d<N>& p)  {return vec3d<>(p)/len(p);}

template <typename N1, typename N2>
bool operator== (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return p.x==q.x && p.y==q.y && p.z==q.z;
}

template <typename N1, typename N2>
bool operator!= (const vec3d<N1>& p, const vec3d<N2>& q)  {
  return p.x!=q.x || p.y!=q.y || p.z!=q.z;
}

template <typename N1, typename N2>
bool operator|| (const vec3d<N1>& p, const vec3d<N2>& q)  {
  vec3d<N1> a = p^q;
  return a.x==0 && a.y==0 && a.z==0;
}

template <typename N1, typename N2>
Number angle(const vec3d<N1>& u, const vec3d<N2>& v)  {
  vec3d<> a = unit(u), b = unit(v);
  return atan2(len(a^b),a*b);
}

template <typename N>
vec3d<N> operator& (const vec3d<N>& a, const quatrn& q)  {
/*
  Number a2 = GSQR(q.r), b2 = GSQR(q.x), c2 = GSQR(q.y), d2 = GSQR(q.z),
         ab = q.r*q.x, ac = q.r*q.y, ad = q.r*q.z,
         bc = q.x*q.y, bd = q.x*q.z, cd = q.y*q.z;
  return vec3d<N>((a2+b2-c2-d2)*a.x + 2*(bc-ad)    *a.y + 2*(bd+ac)    *a.z,
                  2*(bc+ad)    *a.x + (a2-b2+c2-d2)*a.y + 2*(cd-ab)    *a.z,
                  2*(bd-ac)    *a.x + 2*(cd+ab)    *a.y + (a2-b2-c2+d2)*a.z);
*/
  Number p1 = (q.r-q.z)*(q.r+q.z), p2 = (q.x-q.y)*(q.x+q.y),
         ab = q.r*q.x, ac = q.r*q.y, ad = q.r*q.z,
         bc = q.x*q.y, bd = q.x*q.z, cd = q.y*q.z;
  return vec3d<N>((p1+p2)  *a.x + 2*(bc-ad)*a.y + 2*(bd+ac)         *a.z,
                  2*(bc+ad)*a.x + (p1-p2)  *a.y + 2*(cd-ab)         *a.z,
                  2*(bd-ac)*a.x + 2*(cd+ab)*a.y + (4*GSQR(q.r)-2*p1-1)*a.z);
}

template <typename N1, typename N2>
vec3d<N1> operator& (const vec3d<N1>& u, const std::pair< N2,vec3d<N2> >& p)  {
  return u & quatrn(p.first,p.second);
}

template <typename N1, typename N2>
std::pair<Number,quatrn> operator/ (const vec3d<N1>& u, const vec3d<N2>& v)  {
  Number du = len(u), dv = len(v);
  vec3d<> a = u/len(u), b = v/len(v), t = b^a;
  Number s = len(t), p = atan2(s,b*a)/2;
  t *= sin(p)/s;
  return std::make_pair(du/dv,quatrn(cos(p),t.x,t.y,t.z));
}
}
#undef GSQR
#endif
