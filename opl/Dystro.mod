/*********************************************
 * OPL 22.1.2.0 Model
 * Author: Kacper79
 * Creation Date: 16 Mar 2026 at 20:01:49
 *********************************************/
// =============================
// Distribution 1 - H. Williams
// Minimal cost distribution
// =============================

{string} Factories = ...;
{string} Depots    = ...;
{string} Customers = ...;

tuple ArcFD {
  string f;
  string d;
}

tuple ArcFC {
  string f;
  string c;
}

tuple ArcDC {
  string d;
  string c;
}

{ArcFD} FD = ...;   // factory -> depot
{ArcFC} FC = ...;   // factory -> customer
{ArcDC} DC = ...;   // depot   -> customer

float capF[Factories] = ...;
float capD[Depots]    = ...;
float dem[Customers]  = ...;

float costFD[FD] = ...;
float costFC[FC] = ...;
float costDC[DC] = ...;

// decision variables
dvar float+ x[FD];
dvar float+ y[FC];
dvar float+ z[DC];

// objective
dexpr float TotalCost =
    sum(a in FD) costFD[a] * x[a]
  + sum(a in FC) costFC[a] * y[a]
  + sum(a in DC) costDC[a] * z[a];

minimize TotalCost;

// constraints
subject to {

  // Factory capacities
  forall(f in Factories)
    sum(a in FD : a.f == f) x[a]
  + sum(a in FC : a.f == f) y[a]
  <= capF[f];

  // Depot capacities (inbound throughput)
  forall(d in Depots)
    sum(a in FD : a.d == d) x[a]
    <= capD[d];

  // Flow balance in depots
  forall(d in Depots)
    sum(a in DC : a.d == d) z[a]
    ==
    sum(a in FD : a.d == d) x[a];

  // Customer demand satisfaction
  forall(c in Customers)
    sum(a in FC : a.c == c) y[a]
  + sum(a in DC : a.c == c) z[a]
    == dem[c];
}

// output
execute {
  writeln("==================================");
  writeln("Optimal total cost = ", cplex.getObjValue(), " (pounds/month for tonne)");
  writeln("==================================");

  writeln("\nFactory -> Depot");
  for(var a in FD)
    if (x[a] > 1e-6)
      writeln(a.f, " -> ", a.d, " : ", x[a]);

  writeln("\nFactory -> Customer");
  for(var a in FC)
    if (y[a] > 1e-6)
      writeln(a.f, " -> ", a.c, " : ", y[a]);

  writeln("\nDepot -> Customer");
  for(var a in DC)
    if (z[a] > 1e-6)
      writeln(a.d, " -> ", a.c, " : ", z[a]);
}