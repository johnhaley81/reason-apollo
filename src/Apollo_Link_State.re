open Apollo_Types;

module Context = {
  type t;

  [@bs.get] external cacheGet: t => apolloCache = "cache";
};

module Resolver = {
  type root;
  type info;
  type t = (root, Js.Json.t, Context.t, info) => Js.Json.t;

  let make = (~fieldName, resolver) => (fieldName, resolver);
};

module Resolvers = {
  type t = Js.Dict.t(Resolver.t);

  let make = resolvers => Js.Dict.fromList(resolvers);
};

type resolvers = {
  .
  "Query": Resolvers.t,
  "Mutation": Resolvers.t,
};

type linkOptions = {
  .
  "defaults": Js.Json.t,
  "cache": apolloCache,
  "resolvers": resolvers,
};

/* Bind the withClientState method */
[@bs.module "apollo-link-state"]
external make_: linkOptions => apolloLink = "withClientState";

/**
 * CreateContextLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-context
 */
let make =
    (
      ~defaults,
      ~mutations=Resolvers.make([]),
      ~queries=Resolvers.make([]),
      cache,
    ) =>
  /* Instanciate a new context link object */
  make_({
    "defaults": defaults,
    "cache": cache,
    "resolvers": {
      "Query": queries,
      "Mutation": mutations,
    },
  });
