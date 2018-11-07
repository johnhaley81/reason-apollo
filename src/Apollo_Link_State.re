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
  "cache": apolloCache,
  "typeDefs": string,
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
      ~mutations=Resolvers.make([]),
      ~queries=Resolvers.make([]),
      ~typeDefs,
      cache,
    ) =>
  /* Instanciate a new context link object */
  make_({
    "cache": cache,
    "typeDefs": typeDefs,
    "resolvers": {
      "Query": queries,
      "Mutation": mutations,
    },
  });
