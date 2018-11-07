open Apollo_Types;

/* Bind the setContext method */
[@bs.module "apollo-link-context"]
external make_: (unit => Js.t({..})) => apolloLink = "setContext";

/**
 * CreateContextLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-context
 */
let make = contextHandler =>
  /* Instanciate a new context link object */
  make_(contextHandler);
