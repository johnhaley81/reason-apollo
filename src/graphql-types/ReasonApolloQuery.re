open ReasonApolloTypes;

module Get = (Config: ReasonApolloTypes.Config) => {
  [@bs.module] external gql: ReasonApolloTypes.gql = "graphql-tag";
  [@bs.module "react-apollo"]
  external queryComponent: ReasonReact.reactClass = "Query";
  type response = queryResponse(Config.t);

  type renderPropObj = {
    result: response,
    data: option(Config.t),
    error: option(apolloError),
    loading: bool,
    refetch: option(Js.Json.t) => Js.Promise.t(response),
    fetchMore:
      (~variables: Js.Json.t=?, ~updateQuery: updateQueryT, unit) =>
      Js.Promise.t(unit),
    networkStatus: int,
    subscribeToMore:
      (
        ~document: queryString,
        ~variables: Js.Json.t=?,
        ~updateQuery: updateQuerySubscriptionT=?,
        ~onError: onErrorT=?,
        unit,
        unit
      ) =>
      unit,
  };

  let graphqlQueryAST = gql(. Config.query);
  let apolloDataToVariant: queryRenderPropObjJS => response =
    apolloData =>
      switch (
        apolloData->loadingGet,
        apolloData->dataGet |> Js.Nullable.toOption,
        apolloData->errorGet |> Js.Nullable.toOption,
      ) {
      | (true, _, _) => Loading
      | (false, Some(response), _) => Data(Config.parse(response))
      | (false, _, Some(error)) => Error(error)
      | (false, None, None) =>
        Error({
          "message": "No data",
          "graphQLErrors": Js.Nullable.null,
          "networkError": Js.Nullable.null,
        })
      };

  let convertJsInputToReason = (apolloData: queryRenderPropObjJS) => {
    result: apolloData |> apolloDataToVariant,
    data:
      switch (apolloData->dataGet |> ReasonApolloUtils.getNonEmptyObj) {
      | None => None
      | Some(data) =>
        switch (Config.parse(data)) {
        | parsedData => Some(parsedData)
        | exception _ => None
        }
      },
    error:
      switch (apolloData->errorGet |> Js.Nullable.toOption) {
      | Some(error) => Some(error)
      | None => None
      },
    loading: apolloData->loadingGet,
    refetch: variables =>
      apolloData->(refetchGet(variables |> Js.Nullable.fromOption))
      |> Js.Promise.then_(data =>
           data |> apolloDataToVariant |> Js.Promise.resolve
         ),
    fetchMore: (~variables=?, ~updateQuery, ()) =>
      apolloData
      ->(fetchMoreGet(fetchMoreOptions(~variables?, ~updateQuery, ()))),
    networkStatus: apolloData->networkStatusGet,
    subscribeToMore:
      (~document, ~variables=?, ~updateQuery=?, ~onError=?, ()) =>
      apolloData
      ->(
          subscribeToMoreGet(
            subscribeToMoreOptions(
              ~document,
              ~variables?,
              ~updateQuery?,
              ~onError?,
              (),
            ),
          )
        ),
  };

  let make =
      (
        ~client: option(generatedApolloClient)=?,
        ~variables: option(Js.Json.t)=?,
        ~pollInterval: option(int)=?,
        ~notifyOnNetworkStatusChange: option(bool)=?,
        ~fetchPolicy: option(string)=?,
        ~errorPolicy: option(string)=?,
        ~ssr: option(bool)=?,
        ~displayName: option(string)=?,
        ~delay: option(bool)=?,
        ~context: option(Js.Json.t)=?,
        children: renderPropObj => ReasonReact.reactElement,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=queryComponent,
      ~props=
        Js.Nullable.{
          "query": graphqlQueryAST,
          "client": client |> fromOption,
          "variables": variables |> fromOption,
          "pollInterval": pollInterval |> fromOption,
          "notifyOnNetworkStatusChange":
            notifyOnNetworkStatusChange |> fromOption,
          "fetchPolicy": fetchPolicy |> fromOption,
          "errorPolicy": errorPolicy |> fromOption,
          "ssr": ssr |> fromOption,
          "displayName": displayName |> fromOption,
          "delay": delay |> fromOption,
          "context": context |> fromOption,
        },
      apolloData =>
      apolloData |> convertJsInputToReason |> children
    );
};
