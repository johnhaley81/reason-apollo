/**
 * An abstract type to describe a query string object.
 */
type queryString;

/**
 * The signature of the `graphql-tag/gql` function that transforms a GraphQL
 * query string to the standard GraphQL AST.
 * https://github.com/apollographql/graphql-tag
 */
type gql = (. string) => queryString;

/**
 * An abstract type to describe an Apollo Link object.
 */
type apolloLink;

/**
 * An abstract type to describe an Apollo Cache object.
 */
type apolloCache;

type networkError = {. "statusCode": int};

type graphqlError = {
  .
  "message": string,
  "locations": Js.Nullable.t(array(string)),
  "path": Js.Nullable.t(array(string)),
  "nodes": Js.Nullable.t(array(string)),
};

type executionResult = {
  .
  "errors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "data": Js.Nullable.t(Js.Json.t),
};

/* TODO define all types */
type operation = {. "query": queryString};

/* TODO define subscription */
type subscription;

type errorResponse = {
  .
  "graphQLErrors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "networkError": Js.Nullable.t(networkError),
  "response": Js.Nullable.t(executionResult),
  "operation": operation,
  "forward": operation => subscription,
};

module type Config = {
  let query: string;
  type t;
  let parse: Js.Json.t => t;
};

type apolloError = {
  .
  "message": string,
  "graphQLErrors": Js.Nullable.t(array(string)),
  "networkError": Js.Nullable.t(string),
};

type apolloOptions = {
  .
  "query": queryString,
  "variables": Js.Json.t,
};

type queryResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a);

type mutationResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a)
  | NotCalled;

type subscriptionResponse('a) =
  | Loading
  | Error(apolloError)
  | Data('a);

/*
 apollo link ws
 */

[@bs.deriving abstract]
type webSocketLinkOptionsT = {
  [@bs.optional]
  reconnect: bool,
};

[@bs.deriving abstract]
type webSocketLinkT = {
  uri: string,
  options: webSocketLinkOptionsT,
};

type documentNodeT;

type splitTest = {. "query": documentNodeT};

type queryObj = {
  .
  "query": queryString,
  "variables": Js.Json.t,
};

type mutationObj = {
  .
  "mutation": queryString,
  "variables": Js.Json.t,
};

type onErrorT;

[@bs.deriving abstract]
type updateQueryOptions = {
  [@bs.optional]
  fetchMoreResult: Js.Json.t,
  [@bs.optional]
  variables: Js.Json.t,
};

type updateQueryT = (Js.Json.t, updateQueryOptions) => Js.Json.t;

type updateSubscriptionOptions = {
  .
  "subscriptionData": Js.Nullable.t(Js.Json.t),
  "variables": Js.Nullable.t(Js.Json.t),
};
type updateQuerySubscriptionT =
  (Js.Json.t, updateSubscriptionOptions) => Js.Json.t;

/* We dont accept a new query for now */
[@bs.deriving abstract]
type fetchMoreOptions = {
  [@bs.optional]
  variables: Js.Json.t,
  updateQuery: updateQueryT,
};

[@bs.deriving abstract]
type subscribeToMoreOptions = {
  document: queryString,
  [@bs.optional]
  variables: Js.Json.t,
  [@bs.optional]
  updateQuery: updateQuerySubscriptionT,
  [@bs.optional]
  onError: onErrorT,
};

[@bs.deriving abstract]
type queryRenderPropObjJS = {
  loading: bool,
  data: Js.Nullable.t(Js.Json.t),
  error: Js.Nullable.t(apolloError),
  refetch:
    Js.Null_undefined.t(Js.Json.t) => Js.Promise.t(queryRenderPropObjJS),
  networkStatus: int,
  variables: Js.Null_undefined.t(Js.Json.t),
  fetchMore: fetchMoreOptions => Js.Promise.t(unit),
  subscribeToMore: (subscribeToMoreOptions, unit) => unit,
};

type mutationRenderPropObjJS = {
  .
  "loading": bool,
  "called": bool,
  "data": Js.Nullable.t(Js.Json.t),
  "error": Js.Nullable.t(apolloError),
  "networkStatus": int,
  "variables": Js.Null_undefined.t(Js.Json.t),
};

type generatedApolloClient = {
  .
  "query": [@bs.meth] (queryObj => Js.Promise.t(queryRenderPropObjJS)),
  "mutate": [@bs.meth] (mutationObj => Js.Promise.t(mutationRenderPropObjJS)),
  "resetStore": [@bs.meth] (unit => unit),
};
